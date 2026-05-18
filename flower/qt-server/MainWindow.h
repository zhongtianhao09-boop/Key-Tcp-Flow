#pragma once
#include "../OCCIOP/OCCIOP.h"
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnect();
    void onDisconnect();
    void onRefresh();
    void onTabChanged(int index);

    // SECNODE
    void onInsertNode();
    void onUpdateNode();
    void onDeleteNode();

    // SECKEYINFO
    void onInsertKeyInfo();
    void onUpdateKeyInfo();
    void onDeleteKeyInfo();

    // KEYSN
    void onUpdateKeysn();

    // SRVCFG
    void onInsertSrvCfg();
    void onUpdateSrvCfg();
    void onDeleteSrvCfg();

private:
    void setupUI();
    void setupConnPanel(QWidget *container);
    void updateConnState(bool connected);

    void setupNodeTab();
    void setupKeyInfoTab();
    void setupKeysnTab();
    void setupSrvCfgTab();
    void setupTranTab();

    void refreshNodeTable();
    void refreshKeyInfoTable();
    void refreshKeysnTable();
    void refreshSrvCfgTable();
    void refreshTranTable();

    // 连接面板
    QLineEdit *m_hostEdit;
    QLineEdit *m_userEdit;
    QLineEdit *m_passEdit;
    QPushButton *m_connBtn;
    QPushButton *m_discBtn;
    QLabel *m_statusLabel;

    QTabWidget *m_tabs;

    // SECNODE
    QTableWidget *m_nodeTable;
    QLineEdit *m_nodeIdEdit;
    QLineEdit *m_nodeNameEdit;
    QLineEdit *m_nodeDescEdit;

    // SECKEYINFO
    QTableWidget *m_keyInfoTable;
    QLineEdit *m_kiClientEdit;
    QLineEdit *m_kiServerEdit;
    QLineEdit *m_kiKeyEdit;

    // KEYSN
    QTableWidget *m_keysnTable;
    QLineEdit *m_keysnEdit;

    // SRVCFG
    QTableWidget *m_srvCfgTable;
    QLineEdit *m_srvKeyEdit;
    QLineEdit *m_srvValEdit;

    // TRAN
    QTableWidget *m_tranTable;

    OCCIOP *m_occi;
    bool m_connected;
};
