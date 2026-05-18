#include "SecKeyShm.h"
#include "../SecKeyNodeInfo/SeckeyNodeInfo.h"
#include <bits/stdc++.h>
using namespace std;
SecKeyShm::SecKeyShm(int key, int maxNode)
    : BaseShm(key, maxNode * sizeof(NodeSecKeyInfo)), m_maxNode(maxNode){};
SecKeyShm::SecKeyShm(string pathName, int maxNode)
    : BaseShm(pathName, maxNode * sizeof(NodeSecKeyInfo)), m_maxNode(maxNode){};
SecKeyShm::~SecKeyShm(){};
void SecKeyShm::shmInit() {
  if (m_shmAddr != NULL) {
    memset(m_shmAddr, 0, m_maxNode * sizeof(NodeSecKeyInfo));
  } // string 不能用memset
};
int SecKeyShm::shmWrite(NodeSecKeyInfo *pNodeInfo) {
  int ret = -1;
  NodeSecKeyInfo *pAddr = static_cast<NodeSecKeyInfo *>(mapShm());
  if (pAddr == NULL) {
    return ret;
  };
  NodeSecKeyInfo *pNode = NULL;
  for (int i = 0; i < m_maxNode; i++) {
    pNode = pAddr + i;
    cout << i << endl;
    cout << "clientID 比较:" << pNode->clientID << "," << pNodeInfo->clientID
         << endl;
    cout << "serverID 比较:" << pNode->serverID << "," << pNodeInfo->serverID
         << endl;
    if (strcmp(pNode->clientID, pNodeInfo->clientID) == 0 &&
        strcmp(pNode->serverID, pNodeInfo->serverID) == 0) {
      memcpy(pNode, pNodeInfo, sizeof(NodeSecKeyInfo));
      unmapShm();
      cout << "写入数据成功 : 原数据被覆盖!" << endl;
      return 0;
    }
  }
  int i = 0;
  NodeSecKeyInfo tmpNodeInfo;
  for (i = 0; i < m_maxNode; i++) {
    pNode = pAddr + i;
    if (memcmp(&tmpNodeInfo, pNode, sizeof(NodeSecKeyInfo)) == 0) {
      ret = 0;
      memcpy(pNode, pNodeInfo, sizeof(NodeSecKeyInfo));
      cout << "写入数据成功:在新的节点上添加数据！" << endl;
      break;
    }
  };
  if (i == m_maxNode) {
    ret = -1;
  };
  unmapShm();
  return ret;
};
NodeSecKeyInfo SecKeyShm::shmRead(string clientID, string serverID) {
  NodeSecKeyInfo info;
  NodeSecKeyInfo *pAddr = static_cast<NodeSecKeyInfo *>(mapShm());
  if (pAddr == NULL) {
    return info;
  };
  NodeSecKeyInfo *pNode = NULL;
  for (int i = 0; i < m_maxNode; i++) {
    pNode = pAddr + i;
    cout << i << endl;
    cout << "clientID 比较:" << pNode->clientID << "," << clientID << endl;
    cout << "serverID 比较:" << pNode->serverID << "," << serverID << endl;
    if (strcmp(pNode->clientID, clientID.c_str()) == 0 &&
        strcmp(pNode->serverID, serverID.c_str()) == 0) {
      memcpy(&info, pNode, sizeof(NodeSecKeyInfo));
      cout << "读取数据成功!" << endl;
      break;
    }
  };
  unmapShm();
  return info;
};
