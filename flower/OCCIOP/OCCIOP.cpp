#include "OCCIOP.h"
#include "flower/SecKeyNodeInfo/SeckeyNodeInfo.h"
#include <cstring>
#include <iostream>

OCCIOP::OCCIOP() : m_env(nullptr), m_conn(nullptr) {}

OCCIOP::~OCCIOP() {
  if (m_conn && m_env) {
    colseDB();
  }
}

bool OCCIOP::connectDB(string user, string passwd, string conner) {
  m_env = Environment::createEnvironment("utf8", "utf8");
  m_conn = m_env->createConnection(user, passwd, conner);
  if (m_conn == NULL) {
    return false;
  };
  cout << "数据库连接成功..." << endl;
  return true;
};

void OCCIOP::colseDB() {
  m_env->terminateConnection(m_conn);
  Environment::terminateEnvironment(m_env);
}

string OCCIOP::getCurTime() {
  time_t timep;
  time(&timep);
  char tmp[64];
  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
  return tmp;
}

int OCCIOP::getKeyID() {
  Statement *stat = m_conn->createStatement();
  stat->setSQL("SELECT ikeysn FROM SECMNG.KEYSN FOR UPDATE");
  ResultSet *rs = stat->executeQuery();
  int keyID = -1;
  if (rs->next()) {
    keyID = rs->getInt(1);
  }
  stat->closeResultSet(rs);
  m_conn->terminateStatement(stat);
  return keyID;
}

bool OCCIOP::updateKeyID(int keyID) {
  string sql = "update SECMNG.KEYSN set ikeysn = " + to_string(keyID);
  Statement *stat = m_conn->createStatement(sql);
  stat->setAutoCommit(true);
  int ret = stat->executeUpdate();
  m_conn->terminateStatement(stat);
  if (ret <= 0) {
    return false;
  };
  return true;
}

bool OCCIOP::writeSecKeyID(NodeSecKeyInfo *pNode) {
  char sql[1024] = {0};
  sprintf(sql,
          "INSERT INTO SECMNG.SECKEYINFO(clientid, serverid, keyid, "
          "createtime, state, seckey) "
          "VALUES ('%s', '%s', %d, to_date('%s', 'yyyy-mm-dd hh24:mi:ss'), %d, "
          "'%s')",
          pNode->clientID, pNode->serverID, pNode->seckeyID,
          getCurTime().c_str(), 1, pNode->seckey);
  cout << "insert sql: " << sql << endl;
  Statement *stat = m_conn->createStatement();
  stat->setSQL(sql);
  stat->setAutoCommit(true);
  bool bl = stat->executeUpdate(sql);
  m_conn->terminateStatement(stat);
  if (bl) {
    return true;
  };
  return false;
};
