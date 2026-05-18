# Flower 任务规划

## 源码现状分析

### 已有模块

**1. Codec 编解码体系**（`Codec.h` → `RequestCodec.h` / `RespondCodec.h`）
- 抽象基类 `Codec`：虚函数 `encodeMsg()` → `string`、`decodeMsg()` → `void*`
- `RequestCodec`：`RequestInfo` ↔ `RequestMsg`（protobuf） ↔ 二进制
- `RespondCodec`：`RespondInfo` ↔ `RespondMsg`（protobuf） ↔ 二进制
- 问题：`decodeMsg()` 返回 `void*`，类型不安全；调用者需手动 cast 和 delete

**2. TCP 通信**（`TcpServer_w/TcpServer.*`, `TcpSocket.*`）
- `TcpSocket`：创建/包装 fd，`sendMsg`/`recvMsg`（4 字节长度头），`readn`/`writen` 可靠读写
- `TcpServer`：`setListen`（SO_REUSEADDR），`acceptConn`（poll 超时），返回 `TcpSocket*`
- 问题：只 poll 单个监听 fd，无连接管理，无 epoll

**3. 线程池**（`ThreadPool.h` + `cp/ThreadPool.cpp`）
- `enqueue(F&&, Args...)` → `future<invoke_result_t>`
- condition_variable 休眠唤醒，RAII 析构 join

### 源码问题清单

| # | 文件 | 问题 |
|---|------|------|
| 1 | `Codec.h:5/RequestCodec.h:7` | `using namespace std;` 在头文件中，污染所有 includer |
| 2 | `Codec.h:10-11` | `decodeMsg()` 返回 `void*`，类型不安全 |
| 3 | `RequestCodec.cpp:33` / `RespondCodec.cpp:33` | `decodeMsg()` 内 `new` 返回裸指针，内存泄漏风险 |
| 4 | `TcpSocket.cpp:67` | `recvMsg()` 用 `new char[len+1]` 裸分配 |
| 5 | `TcpServer.cpp:18-41` | `setListen` 每步失败只返回 -1，无错误信息 |
| 6 | `TcpServer` | 只 poll 单个 lfd，无连接管理、无 epoll |
| 7 | `test_tcp.cpp` | 直接用 `std::thread`，未使用 ThreadPool |
| 8 | 全局 | 三个模块完全独立，无集成 |

## 阶段规划

### 阶段 1：修正源码基础问题

- [ ] 1.1 移除头文件中的 `using namespace std;`，改为 `std::` 前缀
- [ ] 1.2 修改 `decodeMsg()` 接口：从 `void*` 改为返回具体类型指针（`RequestInfo*` / `RespondInfo*`）或 `unique_ptr`
- [ ] 1.3 将 `TcpSocket::recvMsg()` 中的裸 `new[]` 改为 `vector<char>`
- [ ] 1.4 `TcpServer::setListen()` 失败时输出 perror 信息

### 阶段 2：集成并发服务器

- [ ] 2.1 设计 `FlowerServer` 类，聚合 TcpServer + ThreadPool
- [ ] 2.2 定义回调：`using Handler = function<string(const string&)>`（收到序列化数据 → 处理后返回序列化数据）
- [ ] 2.3 主循环：accept → enqueue(Handler, recv+process+send) → detach future
- [ ] 2.4 编写 `test_flower.cpp` 端到端测试

### 阶段 3：epoll 多路复用

- [ ] 3.1 封装 `Epoller` 类（create/add/mod/del/wait）
- [ ] 3.2 TcpServer 增加 epoll 模式：`void run()` 方法 event loop
- [ ] 3.3 编写 `test_epoll.cpp` 多连接并发测试

### 阶段 4：日志模块

- [ ] 4.1 实现 `Logger` 类，支持 DEBUG/INFO/WARN/ERROR/FATAL
- [ ] 4.2 日志替换现有的 cout 调试输出

## 决策记录

| 日期 | 决策 | 原因 |
|------|------|------|
| | | |
