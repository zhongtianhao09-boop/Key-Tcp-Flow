#pragma once
#include "../SecKeyNodeInfo/SeckeyNodeInfo.h"
#include <occi.h>
#include <string>
using namespace std;
using namespace oracle::occi;
class OCCIOP {
public:
  OCCIOP();
  ~OCCIOP();
  bool connectDB(string user, string passwd, string conner);
  int getKeyID();
  bool updateKeyID(int keyID);
  bool writeSecKeyID(NodeSecKeyInfo *pNode);
  void colseDB();
  Connection *getConnection() { return m_conn; }

private:
  string getCurTime();

private:
  Environment *m_env;
  Connection *m_conn;
};