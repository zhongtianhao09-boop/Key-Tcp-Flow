#pragma once
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>

class SecKeyShm;
struct NodeSecKeyInfo;

class ClientWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow() override;

private slots:
    void onAgree();
    void onCheck();
    void onRevoke();
    void onSendData();
    void onLoadConfig();

private:
    void setupUI();
    void appendLog(const QString &msg, bool isError = false);

    // 连接配置
    QLineEdit *m_serverIP;
    QSpinBox *m_port;
    QLineEdit *m_clientID;
    QLineEdit *m_serverID;

    // 操作
    QPushButton *m_agreeBtn;
    QPushButton *m_checkBtn;
    QPushButton *m_revokeBtn;

    // 密钥显示
    QLabel *m_keyLabel;

    // 数据通信
    QLineEdit *m_dataInput;
    QPushButton *m_sendDataBtn;

    // 日志
    QPlainTextEdit *m_log;

    // 共享内存
    SecKeyShm *m_shm = nullptr;
    std::string m_shmKey;
    int m_shmMaxNode = 4;
};
