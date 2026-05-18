#include "ClientOp.h"
#include <cstdio>
#include <iostream>
using namespace std;

int usage();
int main() {
  // 创建客户端操作类对象
  ClientOp op("Client.json");
  while (1) {
    int sel = usage();
    switch (sel) {
    case 1:
      op.seckeyAgree();
      break;
    case 2:
      op.seckeyCheck();
      break;
    case 3:
      op.seckeyZhuXiao();
      break;
    case 4: {
      NodeSecKeyInfo info = op.viewKey();
      if (info.status == 1) {
        cout << "客户ID: " << info.clientID << endl;
        cout << "服务ID: " << info.serverID << endl;
        cout << "密钥: " << info.seckey << endl;
        cout << "密钥ID: " << info.seckeyID << endl;
      } else {
        cout << "未找到有效密钥" << endl;
      }
      break;
    }
    case 5: {
      string input;
      cout << "输入要发送的数据: ";
      getline(cin, input);
      string reply = op.sendData(input);
      if (!reply.empty()) {
        cout << "服务端回复: " << reply << endl;
      }
      break;
    }
    case 0:
      cout << "客户端退出" << endl;
      return 0;
    default:
      break;
    };
  };
};
int usage() {
  int nSel = -1;
  printf("\n "
         "/********************************************************************"
         "************");
  printf("\n "
         "/********************************************************************"
         "************");
  printf("\n /*        1.密钥协商");
  printf("\n /*        2.密钥校验");
  printf("\n /*        3.密钥注销");
  printf("\n /*        4.密钥查看");
  printf("\n /*        5.加密数据通信");
  printf("\n /*        0.退出系统");
  printf("\n "
         "/********************************************************************"
         "************");
  printf("\n "
         "/********************************************************************"
         "************");
  printf("\n\n 选择:");
  scanf("%d", &nSel);
  while (getchar() != '\n')
    ;
  return nSel;
};