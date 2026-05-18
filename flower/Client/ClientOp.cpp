#include "ClientOp.h"
#include "../common/ProtocolConstants.h"
#include "../Myinterface/AesCrypto.h"
#include "../RsaCripto/RsaCripto.h"
#include "../codec/CodecFactory.h"
#include "../codec/RequestCodec.h"
#include "../codec/RequestFactory.h"
#include "../codec/RespondFactory.h"
#include "../tcp/TcpSocket.h"
#include "json.hpp"
#include <fstream>
#include <sstream>
using json = nlohmann::json;
ClientOp::ClientOp(std::string jsonFile) {
  std::ifstream ifs(jsonFile);
  if (!ifs.is_open()) {
    throw std::runtime_error("无法打开配置文件: " + jsonFile);
  }
  json root = json::parse(ifs);
  m_info.ServerID = root["ServerID"].get<std::string>();
  m_info.ClientID = root["ClientID"].get<std::string>();
  m_info.ip = root["ServerIP"].get<std::string>();
  m_info.port = root["Port"].get<unsigned short>();
  string shmKey = root["ShmKey"].get<std::string>();
  int maxNode = root["ShmMaxNode"].get<int>();
  m_shm = new SecKeyShm(shmKey, maxNode);
}

ClientOp::~ClientOp() { delete m_shm; }

bool ClientOp::seckeyAgree() {
  Cryptographic rsa;
  rsa.generateRsakey(1024);
  std::ifstream ifs("public.pem");
  std::stringstream str;
  str << ifs.rdbuf();
  RequestInfo reqInfo;
  reqInfo.clientID = m_info.ClientID;
  reqInfo.serverID = m_info.ServerID;
  reqInfo.cmd = 1;
  reqInfo.data = str.str();
  reqInfo.sign = rsa.rsaSign(str.str());
  cout << "签名完成..." << endl;
  CodecFactory *factory = new RequestFactory(&reqInfo);
  Codec *c = factory->createCodec();
  string encstr = c->encodeMsg();
  delete factory;
  delete c;

  TcpSocket *tcp = new TcpSocket;
  int ret = tcp->connectToHost(m_info.ip, m_info.port);
  if (ret != 0) {
    cout << "连接服务器失败..." << endl;
    return false;
  };
  cout << "连接服务器成功..." << endl;
  tcp->sendMsg(encstr);
  string msg = tcp->recvMsg();

  c = RespondFactory::createCodec(msg);
  RespondInfo *resData = (RespondInfo *)c->decodeMsg();
  if (!resData->status) {
    cout << "密钥协商失败" << endl;
    delete c;
    tcp->disconnect();
    delete tcp;
    return false;
  }
  string key = rsa.rsaPriKeyDecrypt(resData->data);
  cout << "对称加密的密钥：" << key << endl;
  NodeSecKeyInfo info;
  strcpy(info.clientID, m_info.ClientID.data());
  strcpy(info.serverID, m_info.ServerID.data());
  strcpy(info.seckey, key.data());
  info.seckeyID = resData->seckeyID;
  info.status = 1;
  m_shm->shmWrite(&info);
  cout << "密钥已存入共享内存" << endl;

  AesCrypto aes(key);
  string testEnc = aes.aesCBCEncrypto(AES_TEST_STR);
  cout << "AES 加密测试: \"" << AES_TEST_STR << "\" -> " << testEnc.size() << " bytes 密文" << endl;
  string testDec = aes.aesCBCDecrypt(testEnc);
  cout << "AES 解密测试: " << testDec << endl;

  delete c;
  tcp->disconnect();
  delete tcp;
  return true;
}

void ClientOp::seckeyCheck() {
  NodeSecKeyInfo info = m_shm->shmRead(m_info.ClientID, m_info.ServerID);
  if (info.status == 0) {
    cout << "未找到协商密钥，请先进行密钥协商" << endl;
    return;
  }
  string key(info.seckey);
  AesCrypto aes(key);
  string challenge = CHALLENGE_STR;
  string encChallenge = aes.aesCBCEncrypto(challenge);

  RequestInfo reqInfo;
  reqInfo.clientID = m_info.ClientID;
  reqInfo.serverID = m_info.ServerID;
  reqInfo.cmd = 2;
  reqInfo.data = encChallenge;
  reqInfo.sign = "";
  CodecFactory *factory = new RequestFactory(&reqInfo);
  Codec *c = factory->createCodec();
  string encstr = c->encodeMsg();
  delete factory;
  delete c;

  TcpSocket *tcp = new TcpSocket;
  int ret = tcp->connectToHost(m_info.ip, m_info.port);
  if (ret != 0) {
    cout << "连接服务器失败..." << endl;
    return;
  }
  tcp->sendMsg(encstr);
  string msg = tcp->recvMsg();
  c = RespondFactory::createCodec(msg);
  RespondInfo *resData = (RespondInfo *)c->decodeMsg();
  if (resData->status) {
    cout << "密钥校验通过" << endl;
  } else {
    cout << "密钥校验失败" << endl;
  }
  delete c;
  tcp->disconnect();
  delete tcp;
}

void ClientOp::seckeyZhuXiao() {
  NodeSecKeyInfo info = m_shm->shmRead(m_info.ClientID, m_info.ServerID);
  if (info.status == 0) {
    cout << "未找到协商密钥，请先进行密钥协商" << endl;
    return;
  }
  string key(info.seckey);
  AesCrypto aes(key);
  string revokeMsg = REVOKE_STR;
  string encRevoke = aes.aesCBCEncrypto(revokeMsg);

  RequestInfo reqInfo;
  reqInfo.clientID = m_info.ClientID;
  reqInfo.serverID = m_info.ServerID;
  reqInfo.cmd = 3;
  reqInfo.data = encRevoke;
  reqInfo.sign = "";
  CodecFactory *factory = new RequestFactory(&reqInfo);
  Codec *c = factory->createCodec();
  string encstr = c->encodeMsg();
  delete factory;
  delete c;

  TcpSocket *tcp = new TcpSocket;
  int ret = tcp->connectToHost(m_info.ip, m_info.port);
  if (ret != 0) {
    cout << "连接服务器失败..." << endl;
    return;
  }
  tcp->sendMsg(encstr);
  string msg = tcp->recvMsg();
  c = RespondFactory::createCodec(msg);
  RespondInfo *resData = (RespondInfo *)c->decodeMsg();
  if (resData->status) {
    cout << "密钥注销成功" << endl;
    info.status = 0;
    m_shm->shmWrite(&info);
  } else {
    cout << "密钥注销失败" << endl;
  }
  delete c;
  tcp->disconnect();
  delete tcp;
}

std::string ClientOp::sendData(std::string plainText) {
  NodeSecKeyInfo info = m_shm->shmRead(m_info.ClientID, m_info.ServerID);
  if (info.status == 0) {
    cout << "未找到协商密钥，请先进行密钥协商" << endl;
    return "";
  }

  std::string key(info.seckey);
  AesCrypto aes(key);
  std::string cipherText = aes.aesCBCEncrypto(plainText);

  RequestInfo reqInfo;
  reqInfo.clientID = m_info.ClientID;
  reqInfo.serverID = m_info.ServerID;
  reqInfo.cmd = 4;
  reqInfo.data = cipherText;
  reqInfo.sign = "";

  CodecFactory *factory = new RequestFactory(&reqInfo);
  Codec *c = factory->createCodec();
  std::string encstr = c->encodeMsg();
  delete factory;
  delete c;

  TcpSocket *tcp = new TcpSocket;
  int ret = tcp->connectToHost(m_info.ip, m_info.port);
  if (ret != 0) {
    cout << "连接服务器失败..." << endl;
    delete tcp;
    return "";
  }
  tcp->sendMsg(encstr);
  std::string msg = tcp->recvMsg();

  c = RespondFactory::createCodec(msg);
  RespondInfo *resData = (RespondInfo *)c->decodeMsg();
  if (!resData->status) {
    cout << "数据通信失败" << endl;
    delete c;
    tcp->disconnect();
    delete tcp;
    return "";
  }

  std::string decrypted = aes.aesCBCDecrypt(resData->data);
  delete c;
  tcp->disconnect();
  delete tcp;
  return decrypted;
}
