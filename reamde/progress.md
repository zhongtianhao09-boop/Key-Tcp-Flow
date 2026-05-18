# 进度日志

## 2026-05-12

### 会话开始
- 创建 CLAUDE.md，配置 4 个 skill 自动加载（web-access, planning-with-files, simplify, security-review）
- 使用 planning skill 创建 task_plan.md / findings.md / progress.md

### 源码审查
- 通读所有源文件：Codec 体系、TcpServer/TcpSocket、ThreadPool、测试文件
- 列出 8 个源码问题（findings.md 中有详情）
- 制定 4 个阶段的任务规划

## 2026-05-13

### RsaCripto 模块编译修复
- 移除 Windows 专用的 `applink.c` 引用（`extern "C"` 语法错误 + Linux 不存在该文件）
- 修复成员变量名拼写：`m_private` → `m_privateKey`
- 修复构造函数参数名：`filename` → `fileName`
- 修复 `PEM_write_bio_RSApublicKey` 大小写 → `PEM_write_bio_RSAPublicKey`
- 修复 `RSA_generate_key` API 误用（它不接受 RSA* 第一个参数，返回新 RSA*）
- 修复 `BN_free(r)` → `RSA_free(r)`
- 补充缺失的 `#include <openssl/err.h>`
- 修复 `initPublicKey`/`initPrivateKey` 错误路径的 BIO 内存泄漏
- 修复 `initPrivateKey` 参数名 `pubfile` → `prifile`
- 编译通过（剩余 OpenSSL 3.0 deprecation warnings，不影响使用）

### Key/ClientOp 模块编译修复
- `using namespace Json;` → `using json = nlohmann::json;`（nlohmann/json 在 nlohmann 命名空间下）
- `RespondFactory` 构造函数接收 `RespondInfo*` 非 `string`，解码改为 `RespondFactory::createCodec(msg)` 静态方法
- `RespondMsg *resData` → `RespondInfo *resData`（decodeMsg 返回 RespondInfo*）
- `resData->status()` → `resData->status`（RespondInfo 中 status 是 bool 字段非函数）

### 密钥协商流程分析
- `seckeyAgree()` 中 `data` 放临时公钥、`sign` 放签名 — 结构合理
- 问题：签名用了临时私钥而非身份私钥，服务端无法验证发送者身份（已记录，待后续修正）

### 下一步
继续阶段 1：修正 Codec 头文件 using namespace、void* 接口、TcpSocket 裸指针
