# Flower 项目进度记录

最后更新: 2026-05-18

## 2026-05-18 — Myinterface 集成

### Myinterface/AesCrypto 代码质量修复
- 移除 `interface.h`、`AesCrypto.h` 中的 `using namespace std;`
- `AesCrypto.cpp` 中 `new[]`/`delete[]` 替换为 `std::vector<unsigned char>`
- 移除 AesCrypto 中从未使用的 `m_key` 成员，替换为 `m_ready` flag
- `Interface` 重构：`bool direct` hack → `static fromJsonFile()` 工厂方法
- `Interface` 将 `AesCrypto` 存储为成员，避免每次加密/解密重新做密钥扩展
- 创建 `flower/common/ProtocolConstants.h` 集中管理协议挑战串

### AES 加密通信集成
- **ClientOp.cpp**: `seckeyCheck()`/`seckeyZhuXiao()` 改为 AES 加密挑战-应答，不再明文发密钥
- **ServerOP.cpp**: `processClient()` case 2/3 改为 AES 解密验证
- **ClientWindow.cpp (Qt)**: `onCheck()`/`onRevoke()` 同上改造
- `seckeyAgree()`/`onAgree()` 后增加 AES 加解密健全性测试
- `AesCrypto` 源文件编译进 `server`、`connectPool`、`flower-client` 三目标

### 代码审查修复 (simplify)
- **ServerOP**: `workHard()` 消除 70 行重复代码，改为委托 `processClient()`
- 魔术字符串 `"flower_challenge"`/`"flower_revoke"`/`"hello flower"` → 共享常量

## 编译状态

10/10 目标全部通过: `server`, `connectPool`, `flower-qt`, `flower-client`, `myiface`, `test_agree`, `test_sign`, `test_shm`, `testocci`, `libmyinterface.so`

## 全流程测试结果 (7/7 通过)

| # | 测试项 | 结果 | 验证方式 |
|---|--------|------|----------|
| 1 | Oracle 连接 | PASS | `testocci` — OCCI Environment → Connection → SELECT |
| 2 | 共享内存读写 | PASS | `test_shm` — ftok → shmget → shmat → shmWrite → shmRead → 比对 |
| 3 | RSA 签名验签 | PASS | `test_sign` — generateRsakey → Sign → Verify (同对象/重载/编解码后) |
| 4 | Protobuf 编解码 | PASS | `test_sign` — RequestInfo → encode → decode → 字段逐项比对一致 |
| 5 | 服务端启动 | PASS | `connectPool` — Oracle 连接 → ThreadPool(4) → 监听 9898 |
| 6 | 密钥协商 | PASS | `test_agree` — Client RSA签名 → TCP → Server 验签 → Oracle写库 → 共享内存 → AES加密密钥 → 返回 |
| 7 | AES 加解密 | PASS | `myiface` — 读共享内存取密钥 → PKCS7填充 → CBC加密 → 解密还原 "hello flower" |

## 已修复的关键 Bug

1. **ServerOP 编解码器类型错误** — `workHard()` 用 `RespondFactory` 解码 Request，改为 `RequestFactory`
2. **类型转换错误** — `decodeMsg()` 返回 `RequestInfo*`/`RespondInfo*`，错转成 `RequestMsg*`/`RespondMsg*` (4处)
3. **ofstream 未 flush** — `seckeyAgree()` 写 public.pem 后未关闭就读取，加 `{}` 作用域
4. **AES 解密密钥错误** — `AES_set_decrypt_key` 写入了 `&m_encKey`，应为 `&m_decKey`
5. **AES 缺少 PKCS7 padding** — 非16字节对齐数据加解密异常，已补全
6. **BaseShm.h 缺少 include guard** — `#pragma once` 缺失导致重定义
7. **seckeyAgree 内存泄漏** — `new RespondFactory` 未 delete
8. **数据库外键约束** — SECNODE.ID CHAR(4) 不支持 "robin"/"Luffy"，改为 VARCHAR2(32)

## 环境依赖

- **Oracle**: Docker `container-registry.oracle.com/database/free:latest`，端口 1521，PDB=FREEPDB1
  - 启动: `docker start oracle-free`
  - SECMNG/SECMNG 用户，表: SECNODE, SECKEYINFO, KEYSN, SRVCFG, TRAN
- **Oracle Instant Client**: `/home/solid/oracle/instantclient_23_4`
  - 需 `libaio.so.1` symlink: `ln -s libaio.so.1t64.0.2 libaio.so.1`
- **服务端启动**: `cd .claude && LD_LIBRARY_PATH=/home/solid/oracle/instantclient_23_4 ../build-qt/connectPool server.json`
- **共享内存**: ftok 使用 server.json 路径，需先清理旧段: `ipcrm -m <id>`

## 模块清单

```
flower/
├── proto/              # Protobuf 消息定义
├── codec/              # 编解码层 (6个类)
├── RsaCripto/          # RSA 加解密 + 签名验签
├── tcp/                # TCP 通信 (Server + Socket)
├── OCCIOP/             # Oracle OCCI 封装 (connectDB/getKeyID/updateKeyID/writeSecKeyID)
├── ShareMemory/        # BaseShm + SecKeyShm (共享内存)
├── SecKeyNodeInfo/     # NodeSecKeyInfo 数据结构
├── Hash/               # MD5/SHA1/SHA256
├── Myinterface/        # Interface + AesCrypto → libmyinterface.so (动态库)
├── server/             # 服务端 (原始 pthread + 池化 ThreadPool 双模式)
├── Client/             # 命令行客户端 (seckeyAgree/Check/ZhuXiao/View)
├── connectPool/        # ThreadPool/SocketPool/Logger/TaskQuene + pooled server
├── qt-server/          # Qt6 管理后台 (5表CRUD)
├── qt-client/          # Qt6 客户端 GUI (密钥协商/校验/注销 + 日志)
└── tests/              # testocci/test_shm/test_sign/test_agree/test_agree
```

## 待改进项 (非阻塞)

- OpenSSL 3.0 弃用 API (RSA_new/AES_cbc_encrypt等) — 仅警告，功能正常
- SQL 拼接 (qt-server) — 管理后台可接受
- `connectPool/TcpSocket` 与 `tcp/TcpSocket` 重复
- 拼写: `colseDB`→`closeDB`, `TaskQuene`→`TaskQueue`
