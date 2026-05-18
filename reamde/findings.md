# 调研发现

## Codec 设计问题

### decodeMsg() 返回 void*

```cpp
// Codec.h:10
virtual void *decodeMsg();
```

调用者必须手动 cast 和 delete：

```cpp
RequestInfo *decodedReq = (RequestInfo *)reqDecoder.decodeMsg();
// ...use...
delete decodedReq;
```

**改进方向**：返回 `std::unique_ptr<RequestInfo>` 或至少返回具体类型指针，让编译器检查类型而非 void*。

### 头文件 using namespace std

`Codec.h` 和 `RequestCodec.h` 中有 `using namespace std;`，所有包含这些头文件的翻译单元都会被污染。

## TcpSocket 内存管理

### recvMsg() 裸分配

```cpp
// TcpSocket.cpp:67
char *buf = new char[len + 1];
// ...use...
delete[] buf;
```

`std::vector<char>` 可自动管理，异常安全。

### 异常安全

如果在 `readn()` 成功读取长度头后、读取数据体前抛出异常，`new char[]` 会泄漏。

## TcpServer 架构限制

### 当前模型：单连接

```cpp
TcpSocket *conn = server.acceptConn(5000);  // 等一个连接
string msg = conn->recvMsg();               // 读它的数据
conn->sendMsg(reply);                       // 回复
delete conn;                                // 断开
```

不能同时处理多个连接。升级到 epoll 后可在一个线程中管理所有连接的读写事件。

## epoll 实现参考

Linux epoll 三步：`epoll_create1(0)` → `epoll_ctl(add/mod/del)` → `epoll_wait(timeout)`。

封装要点：
- 用 `epoll_event.data.fd` 存储 fd，在事件回调中快速关联到 socket 对象
- 用 `epoll_event.data.ptr` 存储 `TcpSocket*` 指针，适合复杂场景
- 水平触发（LT）更安全，不易丢事件
