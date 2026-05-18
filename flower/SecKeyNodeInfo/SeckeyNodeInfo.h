#pragma once
#include <string.h>
class NodeSecKeyInfo {
public:
  NodeSecKeyInfo() : status(0), seckeyID(0) {
    bzero(clientID, sizeof(clientID));
    bzero(serverID, sizeof(serverID));
    bzero(seckey, sizeof(seckey));
  };
  int status; // 密钥状态 1 可用 0 不可用
  int seckeyID;
  char clientID[12];
  char serverID[12];
  char seckey[128]; // 对称加密的密钥
};