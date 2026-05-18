#pragma once
#include "../ShareMemory/SecKeyShm.h"
#include <string>
struct ClientInfo {
  std::string ServerID;
  std::string ClientID;
  std::string ip;
  unsigned short port;
};
class ClientOp {
public:
  ClientOp(std::string jsonFile);
  ~ClientOp();
  bool seckeyAgree();
  void seckeyCheck();
  void seckeyZhuXiao();
  std::string sendData(std::string plainText);
  NodeSecKeyInfo viewKey() { return m_shm->shmRead(m_info.ClientID, m_info.ServerID); }

private:
  ClientInfo m_info;
  SecKeyShm *m_shm;
};
