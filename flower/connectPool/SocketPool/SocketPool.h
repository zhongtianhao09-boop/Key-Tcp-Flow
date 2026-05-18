#ifndef SOCKETPOOL_H
#define SOCKETPOOL_H

#include "../TcpSocket/TcpSocket.h"
#include <pthread.h>
#include <queue>
#include <string>

class SocketPool {
public:
  SocketPool(std::string ip, unsigned short port, int capacity);
  TcpSocket *getConnec();
  void putConnect(TcpSocket *tcp, bool isValid = true);
  bool isEmpty();
  ~SocketPool();

private:
  void createConnect();

  std::string m_ip;
  int m_capacity;
  int m_nodeNumber;
  unsigned short m_port;
  std::queue<TcpSocket *> m_list;
  pthread_mutex_t m_mutex;
};

#endif
