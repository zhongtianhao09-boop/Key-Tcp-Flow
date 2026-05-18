#pragma once
#include "TcpSocket.h"
class TcpServer {
public:
  TcpServer();
  ~TcpServer();

  int setListen(unsigned short port);
  TcpSocket *acceptConn(int timeout = 10000);
  void closedfd();

private:
  int m_lfd;
};