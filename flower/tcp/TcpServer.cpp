#include "TcpServer.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

// 构造函数，初始化监听套接字为无效值
TcpServer::TcpServer() : m_lfd(-1) {}

// 析构函数，关闭监听套接字
TcpServer::~TcpServer() { closedfd(); }

// 创建服务器套接字并绑定监听端口
int TcpServer::setListen(unsigned short port) {
  // 创建套接字
  m_lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_lfd == -1) return -1;

  // 设置端口复用，避免 TIME_WAIT 后无法立即重启
  int opt = 1;
  setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // 绑定地址和端口
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  int ret = bind(m_lfd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) return -1;

  // 开始监听
  ret = listen(m_lfd, 128);
  if (ret == -1) return -1;

  return 0;
}

// 接受客户端连接，支持超时（默认 10 秒）
// timeout <= 0 表示阻塞等待
TcpSocket *TcpServer::acceptConn(int timeout) {
  // 使用 poll 实现超时等待
  if (timeout > 0) {
    struct pollfd pfd;
    pfd.fd = m_lfd;
    pfd.events = POLLIN;
    int ret = poll(&pfd, 1, timeout);
    if (ret <= 0) return nullptr;
    if (!(pfd.revents & POLLIN)) return nullptr;
  }

  // 接受连接
  struct sockaddr_in clientAddr;
  socklen_t addrLen = sizeof(clientAddr);
  int connfd = accept(m_lfd, (struct sockaddr *)&clientAddr, &addrLen);
  if (connfd == -1) return nullptr;

  return new TcpSocket(connfd);
}

// 关闭监听套接字
void TcpServer::closedfd() {
  if (m_lfd >= 0) {
    close(m_lfd);
    m_lfd = -1;
  }
}
