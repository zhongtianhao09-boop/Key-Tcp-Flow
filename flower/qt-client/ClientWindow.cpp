#include "ClientWindow.h"
#include "../Client/json.hpp"
#include "../common/ProtocolConstants.h"
#include "../Myinterface/AesCrypto.h"
#include "../RsaCripto/RsaCripto.h"
#include "../ShareMemory/SecKeyShm.h"
#include "../SecKeyNodeInfo/SeckeyNodeInfo.h"
#include "../codec/Codec.h"
#include "../codec/CodecFactory.h"
#include "../codec/RequestCodec.h"
#include "../codec/RequestFactory.h"
#include "../codec/RespondCodec.h"
#include "../codec/RespondFactory.h"
#include "../tcp/TcpSocket.h"
#include <QDateTime>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <cstring>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

ClientWindow::ClientWindow(QWidget *parent) : QMainWindow(parent) { setupUI(); }

ClientWindow::~ClientWindow() { delete m_shm; }

void ClientWindow::setupUI() {
    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *mainLayout = new QVBoxLayout(central);

    // ---- 连接配置 ----
    auto *cfgGroup = new QGroupBox("服务器配置");
    auto *cfgLayout = new QVBoxLayout(cfgGroup);
    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("服务器IP:"));
    m_serverIP = new QLineEdit("127.0.0.1");
    row1->addWidget(m_serverIP);
    row1->addWidget(new QLabel("端口:"));
    m_port = new QSpinBox;
    m_port->setRange(1, 65535);
    m_port->setValue(9898);
    row1->addWidget(m_port);
    cfgLayout->addLayout(row1);

    auto *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel("客户端ID:"));
    m_clientID = new QLineEdit("robin");
    row2->addWidget(m_clientID);
    row2->addWidget(new QLabel("服务器ID:"));
    m_serverID = new QLineEdit("Luffy");
    row2->addWidget(m_serverID);
    cfgLayout->addLayout(row2);

    auto *row3 = new QHBoxLayout;
    auto *loadBtn = new QPushButton("加载配置文件");
    connect(loadBtn, &QPushButton::clicked, this, &ClientWindow::onLoadConfig);
    row3->addWidget(loadBtn);
    row3->addStretch();
    cfgLayout->addLayout(row3);

    mainLayout->addWidget(cfgGroup);

    // ---- 操作按钮 ----
    auto *opGroup = new QGroupBox("密钥操作");
    auto *opLayout = new QHBoxLayout(opGroup);
    m_agreeBtn = new QPushButton("密钥协商");
    m_agreeBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 10px "
        "24px; font-size: 14px; }");
    connect(m_agreeBtn, &QPushButton::clicked, this, &ClientWindow::onAgree);
    opLayout->addWidget(m_agreeBtn);

    m_checkBtn = new QPushButton("密钥校验");
    connect(m_checkBtn, &QPushButton::clicked, this, &ClientWindow::onCheck);
    opLayout->addWidget(m_checkBtn);

    m_revokeBtn = new QPushButton("密钥注销");
    connect(m_revokeBtn, &QPushButton::clicked, this, &ClientWindow::onRevoke);
    opLayout->addWidget(m_revokeBtn);

    mainLayout->addWidget(opGroup);

    // ---- 密钥显示 ----
    auto *keyGroup = new QGroupBox("协商密钥");
    auto *keyLayout = new QHBoxLayout(keyGroup);
    m_keyLabel = new QLabel("尚未协商");
    m_keyLabel->setStyleSheet(
        "QLabel { font-family: monospace; font-size: 16px; padding: 8px; "
        "background: #f5f5f5; border: 1px solid #ddd; }");
    m_keyLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    keyLayout->addWidget(m_keyLabel);
    mainLayout->addWidget(keyGroup);

    // ---- 加密数据通信 ----
    auto *dataGroup = new QGroupBox("加密数据通信");
    auto *dataLayout = new QHBoxLayout(dataGroup);
    m_dataInput = new QLineEdit;
    m_dataInput->setPlaceholderText("输入要加密发送的数据...");
    dataLayout->addWidget(m_dataInput);
    m_sendDataBtn = new QPushButton("加密发送");
    connect(m_sendDataBtn, &QPushButton::clicked, this, &ClientWindow::onSendData);
    dataLayout->addWidget(m_sendDataBtn);
    mainLayout->addWidget(dataGroup);

    // ---- 日志区域 ----
    auto *logGroup = new QGroupBox("操作日志");
    auto *logLayout = new QVBoxLayout(logGroup);
    m_log = new QPlainTextEdit;
    m_log->setReadOnly(true);
    m_log->setMaximumBlockCount(1000);
    m_log->setStyleSheet(
        "QPlainTextEdit { font-family: monospace; background: #1e1e1e; color: "
        "#d4d4d4; }");
    logLayout->addWidget(m_log);
    mainLayout->addWidget(logGroup);
}

void ClientWindow::appendLog(const QString &msg, bool isError) {
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString color = isError ? "#f44747" : "#6A9955";
    m_log->appendHtml(
        QString("<span style='color:#888;'>[%1]</span> <span style='color:%2;'>"
                "%3</span>")
            .arg(ts, color, msg.toHtmlEscaped()));
}

// ==================== 加载配置 ====================

void ClientWindow::onLoadConfig() {
    QString path = QFileDialog::getOpenFileName(
        this, "选择客户端配置文件", "", "JSON (*.json)");
    if (path.isEmpty()) return;

    std::ifstream ifs(path.toStdString());
    if (!ifs.is_open()) {
        appendLog("无法打开配置文件: " + path, true);
        return;
    }
    json root = json::parse(ifs);
    m_serverID->setText(QString::fromStdString(root["ServerID"].get<std::string>()));
    m_clientID->setText(QString::fromStdString(root["ClientID"].get<std::string>()));
    m_serverIP->setText(QString::fromStdString(root["ServerIP"].get<std::string>()));
    m_port->setValue(root["Port"].get<unsigned short>());

    m_shmKey = root.value("ShmKey", "");
    m_shmMaxNode = root.value("ShmMaxNode", 4);
    if (!m_shmKey.empty()) {
        delete m_shm;
        m_shm = new SecKeyShm(m_shmKey, m_shmMaxNode);
    }

    appendLog("已加载配置: " + path);
}

// ==================== 密钥协商 ====================

void ClientWindow::onAgree() {
    m_agreeBtn->setEnabled(false);
    appendLog("========== 开始密钥协商 ==========");

    try {
        // 1. 生成 RSA 密钥对
        appendLog("生成 RSA 密钥对 (1024bit)...");
        Cryptographic rsa;
        rsa.generateRsakey(1024);
        appendLog("RSA 密钥对生成完成");

        // 2. 读取公钥
        std::ifstream ifs("public.pem");
        if (!ifs.is_open()) {
            appendLog("公钥文件 public.pem 不存在", true);
            m_agreeBtn->setEnabled(true);
            return;
        }
        std::stringstream str;
        str << ifs.rdbuf();
        std::string pubKey = str.str();
        appendLog("读取公钥文件完毕");

        // 3. 构建请求
        RequestInfo reqInfo;
        reqInfo.clientID = m_clientID->text().toStdString();
        reqInfo.serverID = m_serverID->text().toStdString();
        reqInfo.cmd = 1; // 密钥协商
        reqInfo.data = pubKey;
        reqInfo.sign = rsa.rsaSign(pubKey);
        appendLog("请求签名完成");

        CodecFactory *factory = new RequestFactory(&reqInfo);
        Codec *c = factory->createCodec();
        std::string encstr = c->encodeMsg();
        delete factory;
        delete c;
        appendLog("请求序列化完成");

        // 4. 连接服务器
        appendLog(QString("连接服务器 %1:%2 ...").arg(m_serverIP->text()).arg(
            m_port->value()));
        TcpSocket tcp;
        int ret = tcp.connectToHost(m_serverIP->text().toStdString(),
                                     m_port->value());
        if (ret != 0) {
            appendLog("连接服务器失败", true);
            m_agreeBtn->setEnabled(true);
            return;
        }
        appendLog("连接服务器成功");

        // 5. 发送请求
        tcp.sendMsg(encstr);
        appendLog("已发送协商请求，等待响应...");

        // 6. 接收响应
        std::string msg = tcp.recvMsg();
        appendLog(QString("收到响应 (%1 bytes)").arg(msg.size()));

        c = RespondFactory::createCodec(msg);
        RespondInfo *resData = (RespondInfo *)c->decodeMsg();

        if (!resData->status) {
            appendLog("密钥协商失败: 服务器拒绝", true);
            delete c;
            tcp.disconnect();
            m_agreeBtn->setEnabled(true);
            return;
        }

        // 7. 解密对称密钥
        std::string key = rsa.rsaPriKeyDecrypt(resData->data);
        delete c;
        tcp.disconnect();

        m_keyLabel->setText(QString::fromStdString(key));
        m_keyLabel->setStyleSheet(
            "QLabel { font-family: monospace; font-size: 16px; padding: 8px; "
            "background: #e8f5e9; border: 2px solid #4CAF50; color: #2e7d32; }");
        appendLog("密钥协商成功！对称密钥已获取");

        if (m_shm) {
            NodeSecKeyInfo info;
            strcpy(info.clientID, m_clientID->text().toStdString().data());
            strcpy(info.serverID, m_serverID->text().toStdString().data());
            strcpy(info.seckey, key.data());
            info.seckeyID = resData->seckeyID;
            info.status = 1;
            m_shm->shmWrite(&info);
            appendLog("密钥已存入共享内存");
        }

        AesCrypto aes(key);
        std::string testEnc = aes.aesCBCEncrypto(AES_TEST_STR);
        appendLog(QString("AES 加密测试: \"%1\" -> %2 bytes 密文")
            .arg(AES_TEST_STR).arg(testEnc.size()));
        std::string testDec = aes.aesCBCDecrypt(testEnc);
        appendLog(QString("AES 解密测试: %1").arg(QString::fromStdString(testDec)));
    } catch (std::exception &e) {
        appendLog(QString("异常: %1").arg(e.what()), true);
    }
    m_agreeBtn->setEnabled(true);
}

// ==================== 密钥校验 ====================

void ClientWindow::onCheck() {
    appendLog("========== 密钥校验 ==========");

    try {
        if (!m_shm) { appendLog("未加载配置文件，请先加载配置", true); return; }
        NodeSecKeyInfo info = m_shm->shmRead(m_clientID->text().toStdString(), m_serverID->text().toStdString());
        if (info.status == 0) { appendLog("未找到协商密钥，请先进行密钥协商", true); return; }
        std::string key(info.seckey);
        AesCrypto aes(key);
        std::string challenge = CHALLENGE_STR;
        std::string encChallenge = aes.aesCBCEncrypto(challenge);

        RequestInfo reqInfo;
        reqInfo.clientID = m_clientID->text().toStdString();
        reqInfo.serverID = m_serverID->text().toStdString();
        reqInfo.cmd = 2;
        reqInfo.data = encChallenge;
        reqInfo.sign = "";

        CodecFactory *factory = new RequestFactory(&reqInfo);
        Codec *c = factory->createCodec();
        std::string encstr = c->encodeMsg();
        delete factory;
        delete c;
        appendLog("校验请求构建完成");

        TcpSocket tcp;
        int ret = tcp.connectToHost(m_serverIP->text().toStdString(),
                                     m_port->value());
        if (ret != 0) {
            appendLog("连接服务器失败", true);
            return;
        }

        tcp.sendMsg(encstr);
        std::string msg = tcp.recvMsg();

        c = RespondFactory::createCodec(msg);
        RespondInfo *resData = (RespondInfo *)c->decodeMsg();

        if (resData->status) {
            appendLog("密钥校验通过");
        } else {
            appendLog("密钥校验失败: 密钥无效或已过期", true);
        }
        delete c;
        tcp.disconnect();
    } catch (std::exception &e) {
        appendLog(QString("异常: %1").arg(e.what()), true);
    }
}

// ==================== 密钥注销 ====================

void ClientWindow::onRevoke() {
    appendLog("========== 密钥注销 ==========");

    try {
        if (!m_shm) { appendLog("未加载配置文件，请先加载配置", true); return; }
        NodeSecKeyInfo info = m_shm->shmRead(m_clientID->text().toStdString(), m_serverID->text().toStdString());
        if (info.status == 0) { appendLog("未找到协商密钥，请先进行密钥协商", true); return; }
        std::string key(info.seckey);
        AesCrypto aes(key);
        std::string revokeMsg = REVOKE_STR;
        std::string encRevoke = aes.aesCBCEncrypto(revokeMsg);

        RequestInfo reqInfo;
        reqInfo.clientID = m_clientID->text().toStdString();
        reqInfo.serverID = m_serverID->text().toStdString();
        reqInfo.cmd = 3;
        reqInfo.data = encRevoke;
        reqInfo.sign = "";

        CodecFactory *factory = new RequestFactory(&reqInfo);
        Codec *c = factory->createCodec();
        std::string encstr = c->encodeMsg();
        delete factory;
        delete c;
        appendLog("注销请求构建完成");

        TcpSocket tcp;
        int ret = tcp.connectToHost(m_serverIP->text().toStdString(),
                                     m_port->value());
        if (ret != 0) {
            appendLog("连接服务器失败", true);
            return;
        }

        tcp.sendMsg(encstr);
        std::string msg = tcp.recvMsg();

        c = RespondFactory::createCodec(msg);
        RespondInfo *resData = (RespondInfo *)c->decodeMsg();

        if (resData->status) {
            appendLog("密钥注销成功");
            if (m_shm) {
                info.status = 0;
                m_shm->shmWrite(&info);
            }
            m_keyLabel->setText("已注销");
            m_keyLabel->setStyleSheet(
                "QLabel { font-family: monospace; font-size: 16px; padding: "
                "8px; background: #ffebee; border: 2px solid #f44336; color: "
                "#c62828; }");
        } else {
            appendLog("密钥注销失败", true);
        }
        delete c;
        tcp.disconnect();
    } catch (std::exception &e) {
        appendLog(QString("异常: %1").arg(e.what()), true);
    }
}

// ==================== 加密数据通信 ====================

void ClientWindow::onSendData() {
    QString input = m_dataInput->text().trimmed();
    if (input.isEmpty()) {
        appendLog("请输入要发送的数据", true);
        return;
    }

    if (!m_shm) { appendLog("未加载配置文件，请先加载配置", true); return; }
    NodeSecKeyInfo info = m_shm->shmRead(m_clientID->text().toStdString(), m_serverID->text().toStdString());
    if (info.status == 0) { appendLog("未找到协商密钥，请先进行密钥协商", true); return; }
    std::string key(info.seckey);

    m_sendDataBtn->setEnabled(false);
    appendLog("========== 加密数据通信 ==========");

    try {
        AesCrypto aes(key);
        std::string cipherText = aes.aesCBCEncrypto(input.toStdString());
        appendLog(QString("明文: %1").arg(input));
        appendLog(QString("加密后: %1 bytes").arg(cipherText.size()));

        RequestInfo reqInfo;
        reqInfo.clientID = m_clientID->text().toStdString();
        reqInfo.serverID = m_serverID->text().toStdString();
        reqInfo.cmd = 4;
        reqInfo.data = cipherText;
        reqInfo.sign = "";

        CodecFactory *factory = new RequestFactory(&reqInfo);
        Codec *c = factory->createCodec();
        std::string encstr = c->encodeMsg();
        delete factory;
        delete c;

        TcpSocket tcp;
        int ret = tcp.connectToHost(m_serverIP->text().toStdString(), m_port->value());
        if (ret != 0) {
            appendLog("连接服务器失败", true);
            m_sendDataBtn->setEnabled(true);
            return;
        }

        tcp.sendMsg(encstr);
        std::string msg = tcp.recvMsg();

        c = RespondFactory::createCodec(msg);
        RespondInfo *resData = (RespondInfo *)c->decodeMsg();

        if (!resData->status) {
            appendLog("数据通信失败: 服务器拒绝", true);
            delete c;
            tcp.disconnect();
            m_sendDataBtn->setEnabled(true);
            return;
        }

        std::string decrypted = aes.aesCBCDecrypt(resData->data);
        appendLog(QString("服务端回复: %1").arg(QString::fromStdString(decrypted)));
        delete c;
        tcp.disconnect();
    } catch (std::exception &e) {
        appendLog(QString("异常: %1").arg(e.what()), true);
    }
    m_sendDataBtn->setEnabled(true);
}
