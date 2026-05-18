#ifndef SECKEYSHM_H
#define SECKEYSHM_H
#include "BaseShm.h"
#include "../SecKeyNodeInfo/SeckeyNodeInfo.h"
#include <cstring>
using namespace std;

class NodeSHMInfo {
public:
  NodeSHMInfo() : status(0), seckeyID(0) {
    bzero(clientID, sizeof(clientID));
    bzero(serverID, sizeof(serverID));
    bzero(seckey, sizeof(seckey));
  };
  int status;
  int seckeyID;
  char clientID[12];
  char serverID[12];
  char seckey[128];
};

class SecKeyShm : public BaseShm {
public:
  SecKeyShm(int key, int maxNode);
  SecKeyShm(string pathName, int maxNode);
  ~SecKeyShm();
  void shmInit();
  int shmWrite(NodeSecKeyInfo *pNodeInfo);
  NodeSecKeyInfo shmRead(string clientID, string serverID);

private:
  int m_maxNode;
};
#endif
