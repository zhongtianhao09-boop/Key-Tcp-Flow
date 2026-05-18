#include "ServerOP.h"
#include "../Client/json.hpp"
#include "../common/ProtocolConstants.h"
#include "../Myinterface/AesCrypto.h"
#include "../RsaCripto/RsaCripto.h"
#include "../codec/RequestCodec.h"
#include "../codec/RequestFactory.h"
#include "../codec/RespondCodec.h"
#include "../codec/RespondFactory.h"
#include "../tcp/TcpServer.h"
#include "../tcp/TcpSocket.h"
#include "flower/SecKeyNodeInfo/SeckeyNodeInfo.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <openssl/rand.h>
#include <pthread.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
using json = nlohmann::json;

ServerOP::ServerOP(string jsonFile) {
  std::ifstream ifs(jsonFile);
  if (!ifs.is_open()) {
    throw std::runtime_error("无法打开配置文件: " + jsonFile);
  }
  json root = json::parse(ifs);
  m_port = root["Port"].get<unsigned short>();
  m_serverID = root["ServerID"].get<std::string>();
  m_dbUser = root["dbUser"].get<std::string>();
  m_dbPwd = root["dbPwd"].get<std::string>();
  m_dbConnStr = root["dbConnstr"].get<std::string>();
  m_occi.connectDB(m_dbUser, m_dbPwd, m_dbConnStr);

  string shmKey = root["ShmKey"].get<std::string>();
  int maxNode = root["ShmMaxNode"].get<int>();
  m_shm = new SecKeyShm(shmKey, maxNode);
  srand(time(NULL));
};

ServerOP::~ServerOP() {
  if (m_server) { delete m_server; }
  delete m_shm;
  if (m_logger) { delete m_logger; }
};

void ServerOP::startServer() {
  m_server = new TcpServer;
  m_server->setListen(m_port);
  while (1) {
    cout << "等待客户端连接..." << endl;
    TcpSocket *tcp = m_server->acceptConn();
    if (tcp == NULL) {
      continue;
    };
    // 创建子线程
    pthread_t tid;
    pthread_create(&tid, NULL, workHard, this);
    m_list.insert(make_pair(tid, tcp));
    pthread_detach(tid);
  };
};

void ServerOP::startServerPooled(int threadCount) {
  m_logger = new Logger("flower_server.log", Logger::INFO);
  m_logger->info("Server starting in pooled mode, threads=" + to_string(threadCount));
  m_server = new TcpServer;
  m_server->setListen(m_port);

  ThreadPool pool(threadCount);
  m_logger->info("ThreadPool created, listening on port " + to_string(m_port));

  while (1) {
    TcpSocket *tcp = m_server->acceptConn();
    if (tcp == NULL) continue;
    m_logger->info("Accepted new connection");
    pool.enqueue([this, tcp] { processClient(tcp); });
  }
}

void ServerOP::processClient(TcpSocket *tcp) {
  string msg = tcp->recvMsg();
  if (msg.empty()) { tcp->disconnect(); delete tcp; return; }

  Codec *c = RequestFactory::createCodec(msg);
  RequestInfo *req = (RequestInfo *)c->decodeMsg();

  string data;
  switch (req->cmd) {
  case 1:
    data = seckeyAgree(req);
    if (m_logger) m_logger->info("Key agree: " + req->clientID + " <=> " + req->serverID);
    break;
  case 2: {
    NodeSecKeyInfo keyInfo = m_shm->shmRead(req->clientID, req->serverID);
    RespondInfo resp2;
    resp2.clientID = req->clientID;
    resp2.serverID = m_serverID;
    if (keyInfo.status == 1) {
      std::string key(keyInfo.seckey);
      AesCrypto aes(key);
      std::string decData = aes.aesCBCDecrypt(req->data);
      resp2.status = (decData == CHALLENGE_STR);
    } else {
      resp2.status = false;
    }
    CodecFactory *f2 = new RespondFactory(&resp2);
    Codec *c2 = f2->createCodec();
    data = c2->encodeMsg();
    delete c2; delete f2;
    if (m_logger) m_logger->info("Key check: " + req->clientID + " status=" + to_string(resp2.status));
    break;
  }
  case 3: {
    NodeSecKeyInfo keyInfo = m_shm->shmRead(req->clientID, req->serverID);
    RespondInfo resp3;
    resp3.clientID = req->clientID;
    resp3.serverID = m_serverID;
    if (keyInfo.status == 1) {
      std::string key(keyInfo.seckey);
      AesCrypto aes(key);
      std::string decData = aes.aesCBCDecrypt(req->data);
      if (decData == REVOKE_STR) {
        keyInfo.status = 0;
        m_shm->shmWrite(&keyInfo);
        resp3.status = true;
      } else {
        resp3.status = false;
      }
    } else {
      resp3.status = false;
    }
    CodecFactory *f3 = new RespondFactory(&resp3);
    Codec *c3 = f3->createCodec();
    data = c3->encodeMsg();
    delete c3; delete f3;
    if (m_logger) m_logger->info("Key revoke: " + req->clientID + " status=" + to_string(resp3.status));
    break;
  }
  case 4: {
    NodeSecKeyInfo keyInfo = m_shm->shmRead(req->clientID, req->serverID);
    RespondInfo resp4;
    resp4.clientID = req->clientID;
    resp4.serverID = m_serverID;
    if (keyInfo.status == 1) {
      std::string key(keyInfo.seckey);
      AesCrypto aes(key);
      std::string plainText = aes.aesCBCDecrypt(req->data);
      if (m_logger) m_logger->info("Data received: " + req->clientID + " -> \"" + plainText + "\"");
      std::string reply = "Server received: " + plainText;
      resp4.data = aes.aesCBCEncrypto(reply);
      resp4.status = true;
    } else {
      resp4.status = false;
    }
    CodecFactory *f4 = new RespondFactory(&resp4);
    Codec *c4 = f4->createCodec();
    data = c4->encodeMsg();
    delete c4; delete f4;
    break;
  }
  default:
    break;
  };

  delete c;
  tcp->sendMsg(data);
  tcp->disconnect();
  delete tcp;
}

void *ServerOP::working(void *arg) { return nullptr; };

string ServerOP::seckeyAgree(RequestInfo *reqMsg) {
  { ofstream ofs("public.pem"); ofs << reqMsg->data; }
  RespondInfo info;
  Cryptographic rsa("public.pem", false);
  int ret = rsa.rsaVerify(reqMsg->data, reqMsg->sign);
  if (ret == 0) {
    cout << "sign verify FAIL" << endl;
    info.status = false;
  } else {
    cout << "sign verify PASS" << endl;
    string key = getRandKey(Len16);
    string seckey = rsa.rsaPubKeyEncrypt(key);
    info.clientID = reqMsg->clientID;
    info.data = seckey;
    info.serverID = m_serverID;
    info.status = true;
    NodeSecKeyInfo node;
    strcpy(node.clientID, reqMsg->clientID.data());
    strcpy(node.serverID, reqMsg->serverID.data());
    strcpy(node.seckey, key.data());
    node.seckeyID = m_occi.getKeyID();
    info.seckeyID = node.seckeyID;
    node.status = 1;
    bool bl = m_occi.writeSecKeyID(&node);
    if (bl) {
      m_occi.updateKeyID(node.seckeyID + 1);
      m_shm->shmWrite(&node);
    } else {
      info.status = false;
    };
  }
  CodecFactory *factory = new RespondFactory(&info);
  Codec *c = factory->createCodec();
  string encMsg = c->encodeMsg();
  delete c;
  delete factory;
  return encMsg;
};
void *workHard(void *arg) {
  ServerOP *op = (ServerOP *)arg;
  sleep(1);
  TcpSocket *tcp = op->m_list[pthread_self()];
  if (tcp) {
    op->processClient(tcp);
  }
  op->m_list.erase(pthread_self());
  return NULL;
};
string ServerOP::getRandKey(KeyLen len) {
  // 字符串包含a-z A-Z 0-9,特殊字符
  srand(time(NULL));
  int flag = 0;
  string randStr = string();
  const char *cs = "!@#$%^&*()_+=-;'/.,";
  for (int i = 0; i < len; i++) {
    flag = rand() % 4;
    switch (flag) {
    case 0:
      randStr.append(1, 'a' + rand() % 26);
      break;
    case 1:
      randStr.append(1, 'A' + rand() % 26);
      break;
    case 2:
      randStr.append(1, '0' + rand() % 10);
      break;
    case 3:
      randStr.append(1, cs[rand() % strlen(cs)]);
      break;
    };
  };
  return randStr;
};