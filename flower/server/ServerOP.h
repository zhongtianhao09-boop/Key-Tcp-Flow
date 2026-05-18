#pragma once
#include "../OCCIOP/OCCIOP.h"
#include "../ShareMemory/SecKeyShm.h"
#include "../connectPool/ThreadPool/ThreadPool.h"
#include "../connectPool/TaskQuene/TaskQuene.h"
#include "../connectPool/Logger/Logger.h"
#include "../codec/RequestCodec.h"
#include "../tcp/TcpServer.h"
#include <map>
#include <string>
using namespace std;
class ServerOP {
public:
  enum KeyLen { Len16 = 16, Len24 = 24, Len32 = 32 };
  ServerOP(string json);
  ~ServerOP();
  void startServer();
  void startServerPooled(int threadCount = 4);
  friend void *workHard(void *arg);
  static void *working(void *arg);

private:
  void processClient(TcpSocket *tcp);
  string getRandKey(KeyLen len);
  string seckeyAgree(RequestInfo *reqMsg);

  SecKeyShm *m_shm;
  string m_serverID;
  string m_dbPwd;
  string m_dbUser;
  string m_dbConnStr;
  unsigned short m_port;
  TcpServer *m_server = NULL;
  map<pthread_t, TcpSocket *> m_list;
  Logger *m_logger = nullptr;
  OCCIOP m_occi;
};
void *workHard(void *arg);