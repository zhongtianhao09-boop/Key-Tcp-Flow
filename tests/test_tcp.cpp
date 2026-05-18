// 测试程序：演示 TcpServer + TcpSocket 的完整通信流程
// 服务端在一个子线程中运行，客户端在主线程中连接并收发数据

#include "flower/tcp/TcpServer.h"
#include "flower/tcp/TcpSocket.h"
#include <iostream>
#include <thread>

using namespace std;

// 服务端线程函数
void serverThread() {
  // 1. 创建服务端并监听端口 8888
  TcpServer server;
  server.setListen(8888);
  cout << "[服务端] 已启动，等待连接..." << endl;

  // 2. 接受客户端连接（超时 5 秒）
  TcpSocket *conn = server.acceptConn(5000);
  if (conn == nullptr) {
    cout << "[服务端] 超时，没有客户端连接" << endl;
    return;
  }
  cout << "[服务端] 有客户端连上来了" << endl;

  // 3. 接收客户端发来的消息
  string msg = conn->recvMsg();
  cout << "[服务端] 收到: " << msg << endl;

  // 4. 回复客户端
  string reply = "我收到了: " + msg;
  conn->sendMsg(reply);
  cout << "[服务端] 回复: " << reply << endl;

  // 5. 清理
  conn->closefd();
  delete conn;
  cout << "[服务端] 关闭连接" << endl;
}

int main() {
  // 启动服务端线程
  thread server(serverThread);

  // 等一会儿，确保服务端先启动
  this_thread::sleep_for(chrono::milliseconds(500));

  // 1. 创建客户端套接字
  TcpSocket client;
  cout << "[客户端] 开始连接 127.0.0.1:8888" << endl;

  // 2. 连接服务端
  int ret = client.connectToHost("127.0.0.1", 8888);
  if (ret == -1) {
    cout << "[客户端] 连接失败" << endl;
    return -1;
  }
  cout << "[客户端] 连接成功" << endl;

  // 3. 发送消息给服务端
  client.sendMsg("你好，服务器！");
  cout << "[客户端] 发送: 你好，服务器！" << endl;

  // 4. 接收服务端的回复
  string reply = client.recvMsg();
  cout << "[客户端] 收到回复: " << reply << endl;

  // 5. 关闭客户端
  client.closefd();
  cout << "[客户端] 关闭连接" << endl;

  // 等待服务端线程结束
  server.join();
  return 0;
}
