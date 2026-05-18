#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>

// TCP 套接字封装类，支持客户端和服务端两侧使用
class TcpSocket {
public:
  TcpSocket(); // 创建一个新的 TCP 套接字
  TcpSocket(int fd); // 用已有文件描述符构造（服务端 accept 后的连接）
  ~TcpSocket();

  int connectToHost(const std::string &ip, unsigned short port); // 连接到服务器
  int disconnect();                     // 断开连接（优雅关闭）
  int sendMsg(const std::string &data); // 发送数据（4 字节长度头 + 数据体）
  std::string recvMsg();                // 接收数据（解析长度头）
  void closefd();                       // 关闭套接字

private:
  int readn(char *buf, int size);        // 读取固定 n 字节
  int writen(const char *buf, int size); // 写入固定 n 字节

  int m_fd; // 套接字文件描述符
};

#endif
