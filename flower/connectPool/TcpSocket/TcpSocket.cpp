#include "TcpSocket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

using namespace std;

// 创建一个新的 TCP 套接字
TcpSocket::TcpSocket() { m_fd = socket(AF_INET, SOCK_STREAM, 0); }

// 用已有文件描述符构造（服务端 accept 后使用）
TcpSocket::TcpSocket(int fd) : m_fd(fd) {}

// 关闭并释放套接字
TcpSocket::~TcpSocket() {
  if (m_fd >= 0) {
    close(m_fd);
    m_fd = -1;
  }
}

// 连接到目标服务器
int TcpSocket::connectToHost(const string &ip, unsigned short port) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

  return connect(m_fd, (struct sockaddr *)&addr, sizeof(addr));
}

// 断开连接：优雅关闭 TCP 连接
int TcpSocket::disconnect() {
  if (m_fd >= 0) {
    shutdown(m_fd, SHUT_RDWR);
    close(m_fd);
    m_fd = -1;
  }
  return 0;
}

// 发送数据：先发 4 字节长度头，再发数据体
int TcpSocket::sendMsg(const string &data) {
  // 构造长度头（网络字节序）
  int len = static_cast<int>(data.size());
  int lenNet = htonl(len);

  // 先发长度
  int ret = writen((const char *)&lenNet, 4);
  if (ret == -1)
    return -1;

  // 再发数据
  ret = writen(data.c_str(), len);
  if (ret == -1)
    return -1;

  return 0;
}

// 接收数据：先读 4 字节长度头，再读数据体
string TcpSocket::recvMsg() {
  // 读长度头
  int lenNet = 0;
  int ret = readn((char *)&lenNet, 4);
  if (ret <= 0)
    return string();

  int len = ntohl(lenNet);

  // 读数据体
  char *buf = new char[len + 1];
  ret = readn(buf, len);
  if (ret <= 0) {
    delete[] buf;
    return string();
  }
  buf[len] = '\0';

  string result(buf, len);
  delete[] buf;
  return result;
}

// 关闭套接字
void TcpSocket::closefd() {
  if (m_fd >= 0) {
    close(m_fd);
    m_fd = -1;
  }
}

// -------- 底层读写辅助 -------- //

// 读取固定 n 字节（处理部分读取）
int TcpSocket::readn(char *buf, int size) {
  int total = 0;
  while (total < size) {
    int n = recv(m_fd, buf + total, size - total, 0);
    if (n <= 0)
      return n;
    total += n;
  }
  return total;
}

// 写入固定 n 字节（处理部分写入）
int TcpSocket::writen(const char *buf, int size) {
  int total = 0;
  while (total < size) {
    int n = send(m_fd, buf + total, size - total, 0);
    if (n <= 0)
      return -1;
    total += n;
  }
  return total;
}
