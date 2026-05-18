#include "SocketPool.h"
#include <iostream>

SocketPool::SocketPool(std::string ip, unsigned short port, int capacity)
    : m_capacity(capacity), m_nodeNumber(capacity), m_ip(ip), m_port(port) {
  pthread_mutex_init(&m_mutex, NULL);
  createConnect();
}

SocketPool::~SocketPool() {
  pthread_mutex_lock(&m_mutex);
  while (m_list.size()) {
    TcpSocket *t = m_list.front();
    t->disconnect();
    delete t;
    m_list.pop();
  }
  pthread_mutex_unlock(&m_mutex);
  pthread_mutex_destroy(&m_mutex);
}

void SocketPool::createConnect() {
  static const int MAX_RETRY = 3;
  int retry = 0;

  while (m_list.size() < m_nodeNumber && retry < MAX_RETRY) {
    TcpSocket *tcp = new TcpSocket;
    int ret = tcp->connectToHost(m_ip, m_port);
    if (ret == 0) {
      m_list.push(tcp);
      std::cout << "create new +++++ connect size:" << m_list.size()
                << std::endl;
      retry = 0;
    } else {
      delete tcp;
      retry++;
      std::cout << "connect failed, retry:" << retry << std::endl;
    }
  }
  std::cout << "==current list size:" << m_list.size()
            << " capacity:" << m_nodeNumber << std::endl;
}

TcpSocket *SocketPool::getConnec() {
  pthread_mutex_lock(&m_mutex);
  if (m_list.empty()) {
    pthread_mutex_unlock(&m_mutex);
    return NULL;
  }
  TcpSocket *socket = m_list.front();
  m_list.pop();
  std::cout << "connect size:" << m_list.size() << std::endl;
  pthread_mutex_unlock(&m_mutex);
  return socket;
}

void SocketPool::putConnect(TcpSocket *tcp, bool isValid) {
  bool needCreate = false;

  pthread_mutex_lock(&m_mutex);
  if (isValid) {
    m_list.push(tcp);
    std::cout << "放回的连接可用--------" << m_list.size() << std::endl;
  } else {
    std::cout << "连接不可用--------" << m_list.size() << std::endl;
    tcp->disconnect();
    delete tcp;
    m_nodeNumber = m_list.size() + 1;
    needCreate = true;
  }
  std::cout << "connect  size:" << m_list.size() << std::endl;
  pthread_mutex_unlock(&m_mutex);

  if (needCreate) {
    createConnect();
  }
}

bool SocketPool::isEmpty() {
  pthread_mutex_lock(&m_mutex);
  bool empty = m_list.empty();
  pthread_mutex_unlock(&m_mutex);
  return empty;
}
