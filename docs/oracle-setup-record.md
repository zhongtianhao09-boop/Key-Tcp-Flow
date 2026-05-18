# Oracle 数据库环境修复记录

## 时间

2026-05-14

## 环境

| 组件 | 版本 |
|------|------|
| Oracle 数据库容器 | Docker `oracle-free` — Oracle AI Database 26ai Free (23.26.1.0.0) |
| Oracle Instant Client | 23.4 (路径: `/home/solid/oracle/instantclient_23_4`) |
| JDBC Driver | Oracle JDBC 23.5 (DataGrip 内置) |
| WSL2 IP | `172.22.87.209` (重启后可能变化) |

## 修复内容

### 1. OCCI 版本升级

- **旧**: `/opt/oracle/instantclient_21_15` — 21c Instant Client，无法连接 26ai 数据库
- **新**: `/home/solid/oracle/instantclient_23_4` — 23ai Instant Client，连接正常
- CMakeLists.txt 已更新 `ORACLE_HOME` 路径
- testocci.cpp 已填充 OCCI 连接测试代码

### 2. Windows Oracle 服务冲突

Windows 本机安装的 Oracle 21c XE 监听器占用 localhost:1521，导致 DataGrip 连不到 WSL2 Docker 里的 Oracle。

- `OracleServiceXE` — 已停止 + 已禁用
- `OracleOraDB21Home1TNSListener` — 已停止 + 已禁用

### 3. DataGrip 连接配置

**项目路径**: `C:\Users\Lenovo\DataGripProjects\database\.idea\dataSources.xml`

**连接名**: `o_solid`

| 配置项 | 值 |
|--------|-----|
| JDBC URL | `jdbc:oracle:thin:@//172.22.87.209:1521/FREE` |
| Driver | oracle.19 (Oracle JDBC 23.5) |
| Host | 172.22.87.209 |
| Port | 1521 |
| Service Name | FREE |
| User | SYSTEM |
| Password | flower123 |

`dataSources.local.xml` 中用户名已是 `SYSTEM`。

## 重启后检查清单

### 1. Oracle Docker 容器
```bash
docker start oracle-free
# 等待约 1 分钟启动
docker ps --filter name=oracle-free  # 确认状态 healthy
```

### 2. WSL2 IP（可能变化）
```bash
hostname -I
```
如果 IP 变了，更新 DataGrip 的 JDBC URL 中的 IP 地址。

另外，重启后 Windows localhost 转发通常会恢复正常。可以测试：
```powershell
Test-NetConnection -ComputerName localhost -Port 1521
```
如果能通，把 JDBC URL 改成 `jdbc:oracle:thin:@//localhost:1521/FREE` 即可（更稳定）。

### 3. 验证连接
```bash
cd /home/solid/projects/flower
LD_LIBRARY_PATH=/home/solid/oracle/instantclient_23_4:/tmp ./build/testocci
```

## 项目关键文件

| 文件 | 说明 |
|------|------|
| CMakeLists.txt | ORACLE_HOME 指向 instantclient_23_4 |
| tests/testocci.cpp | OCCI 连接测试代码 |
| flower/oracle_config.txt | Oracle 连接配置记录 |
| build/testocci | 编译后的 OCCI 测试二进制 |
