#include "MainWindow.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_occi(new OCCIOP), m_connected(false) {
    setupUI();
    updateConnState(false);
}

MainWindow::~MainWindow() {
    if (m_connected) m_occi->colseDB();
    delete m_occi;
}

// ==================== UI 构建 ====================

void MainWindow::setupUI() {
    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *mainLayout = new QVBoxLayout(central);

    auto *connGroup = new QGroupBox("数据库连接");
    setupConnPanel(connGroup);
    mainLayout->addWidget(connGroup);

    m_statusLabel = new QLabel("未连接");
    statusBar()->addPermanentWidget(m_statusLabel);

    m_tabs = new QTabWidget;
    setupNodeTab();
    setupKeyInfoTab();
    setupKeysnTab();
    setupSrvCfgTab();
    setupTranTab();
    mainLayout->addWidget(m_tabs);

    connect(m_tabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::setupConnPanel(QWidget *container) {
    auto *layout = new QHBoxLayout(container);
    layout->addWidget(new QLabel("连接串:"));
    m_hostEdit = new QLineEdit("localhost:1521/FREEPDB1");
    layout->addWidget(m_hostEdit);
    layout->addWidget(new QLabel("用户:"));
    m_userEdit = new QLineEdit("SECMNG");
    layout->addWidget(m_userEdit);
    layout->addWidget(new QLabel("密码:"));
    m_passEdit = new QLineEdit("SECMNG");
    m_passEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(m_passEdit);

    m_connBtn = new QPushButton("连接");
    layout->addWidget(m_connBtn);
    connect(m_connBtn, &QPushButton::clicked, this, &MainWindow::onConnect);

    m_discBtn = new QPushButton("断开");
    m_discBtn->setEnabled(false);
    layout->addWidget(m_discBtn);
    connect(m_discBtn, &QPushButton::clicked, this, &MainWindow::onDisconnect);

    auto *refreshBtn = new QPushButton("刷新");
    layout->addWidget(refreshBtn);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefresh);
}

// ==================== SECNODE Tab ====================

void MainWindow::setupNodeTab() {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);

    m_nodeTable = new QTableWidget(0, 6);
    m_nodeTable->setHorizontalHeaderLabels({"网点ID", "名称", "描述", "创建时间", "授权码", "状态"});
    m_nodeTable->horizontalHeader()->setStretchLastSection(true);
    m_nodeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_nodeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_nodeTable);

    auto *form = new QHBoxLayout;
    form->addWidget(new QLabel("ID:"));
    m_nodeIdEdit = new QLineEdit;
    m_nodeIdEdit->setMaxLength(32);
    m_nodeIdEdit->setPlaceholderText("0001");
    form->addWidget(m_nodeIdEdit);
    form->addWidget(new QLabel("名称:"));
    m_nodeNameEdit = new QLineEdit;
    m_nodeNameEdit->setPlaceholderText("网点名称");
    form->addWidget(m_nodeNameEdit);
    form->addWidget(new QLabel("描述:"));
    m_nodeDescEdit = new QLineEdit;
    m_nodeDescEdit->setPlaceholderText("网点描述");
    form->addWidget(m_nodeDescEdit);
    layout->addLayout(form);

    auto *btnRow = new QHBoxLayout;
    auto *ins = new QPushButton("新增网点");
    connect(ins, &QPushButton::clicked, this, &MainWindow::onInsertNode);
    btnRow->addWidget(ins);
    auto *upd = new QPushButton("修改选中");
    connect(upd, &QPushButton::clicked, this, &MainWindow::onUpdateNode);
    btnRow->addWidget(upd);
    auto *del = new QPushButton("删除选中");
    connect(del, &QPushButton::clicked, this, &MainWindow::onDeleteNode);
    btnRow->addWidget(del);
    btnRow->addStretch();
    layout->addLayout(btnRow);

    m_tabs->addTab(page, "SECNODE 网点信息");
}

// ==================== SECKEYINFO Tab ====================

void MainWindow::setupKeyInfoTab() {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);

    m_keyInfoTable = new QTableWidget(0, 6);
    m_keyInfoTable->setHorizontalHeaderLabels({"客户端ID", "服务端ID", "密钥ID", "创建时间", "状态", "密钥"});
    m_keyInfoTable->horizontalHeader()->setStretchLastSection(true);
    m_keyInfoTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_keyInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_keyInfoTable);

    auto *form = new QHBoxLayout;
    form->addWidget(new QLabel("客户端ID:"));
    m_kiClientEdit = new QLineEdit;
    m_kiClientEdit->setPlaceholderText("robin");
    form->addWidget(m_kiClientEdit);
    form->addWidget(new QLabel("服务端ID:"));
    m_kiServerEdit = new QLineEdit;
    m_kiServerEdit->setPlaceholderText("Luffy");
    form->addWidget(m_kiServerEdit);
    form->addWidget(new QLabel("密钥:"));
    m_kiKeyEdit = new QLineEdit;
    m_kiKeyEdit->setPlaceholderText("密钥数据");
    form->addWidget(m_kiKeyEdit);
    layout->addLayout(form);

    auto *btnRow = new QHBoxLayout;
    auto *ins = new QPushButton("新增记录");
    connect(ins, &QPushButton::clicked, this, &MainWindow::onInsertKeyInfo);
    btnRow->addWidget(ins);
    auto *upd = new QPushButton("修改选中");
    connect(upd, &QPushButton::clicked, this, &MainWindow::onUpdateKeyInfo);
    btnRow->addWidget(upd);
    auto *del = new QPushButton("删除选中");
    connect(del, &QPushButton::clicked, this, &MainWindow::onDeleteKeyInfo);
    btnRow->addWidget(del);
    btnRow->addStretch();
    layout->addLayout(btnRow);

    m_tabs->addTab(page, "SECKEYINFO 密钥信息");
}

// ==================== KEYSN Tab ====================

void MainWindow::setupKeysnTab() {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);

    m_keysnTable = new QTableWidget(0, 1);
    m_keysnTable->setHorizontalHeaderLabels({"当前序列号"});
    m_keysnTable->horizontalHeader()->setStretchLastSection(true);
    m_keysnTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_keysnTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_keysnTable);

    auto *form = new QHBoxLayout;
    form->addWidget(new QLabel("新序列号:"));
    m_keysnEdit = new QLineEdit;
    m_keysnEdit->setPlaceholderText("输入新序列号");
    form->addWidget(m_keysnEdit);
    auto *upd = new QPushButton("更新序列号");
    connect(upd, &QPushButton::clicked, this, &MainWindow::onUpdateKeysn);
    form->addWidget(upd);
    form->addStretch();
    layout->addLayout(form);

    m_tabs->addTab(page, "KEYSN 序列号");
}

// ==================== SRVCFG Tab ====================

void MainWindow::setupSrvCfgTab() {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);

    m_srvCfgTable = new QTableWidget(0, 2);
    m_srvCfgTable->setHorizontalHeaderLabels({"键", "值"});
    m_srvCfgTable->horizontalHeader()->setStretchLastSection(true);
    m_srvCfgTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_srvCfgTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_srvCfgTable);

    auto *form = new QHBoxLayout;
    form->addWidget(new QLabel("键:"));
    m_srvKeyEdit = new QLineEdit;
    m_srvKeyEdit->setPlaceholderText("key");
    form->addWidget(m_srvKeyEdit);
    form->addWidget(new QLabel("值:"));
    m_srvValEdit = new QLineEdit;
    m_srvValEdit->setPlaceholderText("value");
    form->addWidget(m_srvValEdit);
    layout->addLayout(form);

    auto *btnRow = new QHBoxLayout;
    auto *ins = new QPushButton("新增配置");
    connect(ins, &QPushButton::clicked, this, &MainWindow::onInsertSrvCfg);
    btnRow->addWidget(ins);
    auto *upd = new QPushButton("修改选中");
    connect(upd, &QPushButton::clicked, this, &MainWindow::onUpdateSrvCfg);
    btnRow->addWidget(upd);
    auto *del = new QPushButton("删除选中");
    connect(del, &QPushButton::clicked, this, &MainWindow::onDeleteSrvCfg);
    btnRow->addWidget(del);
    btnRow->addStretch();
    layout->addLayout(btnRow);

    m_tabs->addTab(page, "SRVCFG 服务配置");
}

// ==================== TRAN Tab ====================

void MainWindow::setupTranTab() {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);

    m_tranTable = new QTableWidget(0, 5);
    m_tranTable->setHorizontalHeaderLabels({"交易ID", "操作员", "交易时间", "交易码", "描述"});
    m_tranTable->horizontalHeader()->setStretchLastSection(true);
    m_tranTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tranTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_tranTable);

    m_tabs->addTab(page, "TRAN 交易记录");
}

// ==================== 连接/断开 ====================

void MainWindow::onConnect() {
    QString host = m_hostEdit->text().trimmed();
    QString user = m_userEdit->text().trimmed();
    QString pass = m_passEdit->text().trimmed();
    if (host.isEmpty() || user.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写连接串和用户名");
        return;
    }
    if (m_occi->connectDB(user.toStdString(), pass.toStdString(), host.toStdString())) {
        m_connected = true;
        updateConnState(true);
        onRefresh();
    } else {
        QMessageBox::critical(this, "失败", "数据库连接失败");
    }
}

void MainWindow::onDisconnect() {
    if (m_connected) { m_occi->colseDB(); m_connected = false; updateConnState(false); }
}

void MainWindow::updateConnState(bool connected) {
    m_hostEdit->setEnabled(!connected);
    m_userEdit->setEnabled(!connected);
    m_passEdit->setEnabled(!connected);
    m_connBtn->setEnabled(!connected);
    m_discBtn->setEnabled(connected);
    m_statusLabel->setText(connected ? "已连接" : "未连接");
}

// ==================== 刷新 ====================

void MainWindow::onRefresh() {
    if (!m_connected) return;
    int idx = m_tabs->currentIndex();
    if (idx == 0) refreshNodeTable();
    else if (idx == 1) refreshKeyInfoTable();
    else if (idx == 2) refreshKeysnTable();
    else if (idx == 3) refreshSrvCfgTable();
    else if (idx == 4) refreshTranTable();
}

void MainWindow::onTabChanged(int) { onRefresh(); }

// ---- SECNODE ----

void MainWindow::refreshNodeTable() {
    if (!m_connected) return;
    m_nodeTable->setRowCount(0);
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "SELECT id, name, nodedesc, TO_CHAR(createtime,'YYYY-MM-DD HH24:MI'), authcode, state FROM SECMNG.SECNODE ORDER BY id");
        auto *rs = s->executeQuery();
        while (rs->next()) {
            int r = m_nodeTable->rowCount();
            m_nodeTable->insertRow(r);
            for (int c = 0; c < 6; c++)
                m_nodeTable->setItem(r, c, new QTableWidgetItem(QString::fromStdString(rs->getString(c + 1))));
        }
        s->closeResultSet(rs);
        m_occi->getConnection()->terminateStatement(s);
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("SECNODE 查询失败:\n") + e.what());
    }
}

void MainWindow::onInsertNode() {
    if (!m_connected) return;
    QString id = m_nodeIdEdit->text().trimmed();
    QString name = m_nodeNameEdit->text().trimmed();
    if (id.isEmpty() || name.isEmpty()) { QMessageBox::warning(this, "提示", "ID和名称不能为空"); return; }
    try {
        char sql[1024];
        snprintf(sql, sizeof(sql), "INSERT INTO SECMNG.SECNODE(id,name,nodedesc,createtime,authcode,state) VALUES ('%s','%s','%s',SYSDATE,1,0)",
                 id.toStdString().c_str(), name.toStdString().c_str(), m_nodeDescEdit->text().trimmed().toStdString().c_str());
        auto *s = m_occi->getConnection()->createStatement(sql);
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        m_nodeIdEdit->clear(); m_nodeNameEdit->clear(); m_nodeDescEdit->clear();
        refreshNodeTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("新增失败:\n") + e.what());
    }
}

void MainWindow::onUpdateNode() {
    if (!m_connected) return;
    int row = m_nodeTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选择行"); return; }
    QString id = m_nodeTable->item(row, 0)->text();
    QString name = m_nodeNameEdit->text().trimmed();
    QString desc = m_nodeDescEdit->text().trimmed();
    if (name.isEmpty() && desc.isEmpty()) { QMessageBox::warning(this, "提示", "请填写名称或描述"); return; }
    try {
        QString sql = "UPDATE SECMNG.SECNODE SET ";
        if (!name.isEmpty()) sql += "name='" + name + "'";
        if (!desc.isEmpty()) { if (!name.isEmpty()) sql += ","; sql += "nodedesc='" + desc + "'"; }
        sql += " WHERE id='" + id + "'";
        auto *s = m_occi->getConnection()->createStatement(sql.toStdString());
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        m_nodeNameEdit->clear(); m_nodeDescEdit->clear();
        refreshNodeTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("修改失败:\n") + e.what());
    }
}

void MainWindow::onDeleteNode() {
    if (!m_connected) return;
    int row = m_nodeTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选择行"); return; }
    QString id = m_nodeTable->item(row, 0)->text();
    if (QMessageBox::question(this, "确认", "确定删除网点 " + id + " 吗？") != QMessageBox::Yes) return;
    try {
        auto *s = m_occi->getConnection()->createStatement("DELETE FROM SECMNG.SECNODE WHERE id='" + id.toStdString() + "'");
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        refreshNodeTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("删除失败:\n") + e.what());
    }
}

// ---- SECKEYINFO ----

void MainWindow::refreshKeyInfoTable() {
    if (!m_connected) return;
    m_keyInfoTable->setRowCount(0);
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "SELECT clientid,serverid,keyid,TO_CHAR(createtime,'YYYY-MM-DD HH24:MI'),state,seckey FROM SECMNG.SECKEYINFO ORDER BY createtime DESC");
        auto *rs = s->executeQuery();
        while (rs->next()) {
            int r = m_keyInfoTable->rowCount();
            m_keyInfoTable->insertRow(r);
            for (int c = 0; c < 6; c++)
                m_keyInfoTable->setItem(r, c, new QTableWidgetItem(QString::fromStdString(rs->getString(c + 1))));
        }
        s->closeResultSet(rs);
        m_occi->getConnection()->terminateStatement(s);
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("SECKEYINFO 查询失败:\n") + e.what());
    }
}

void MainWindow::onInsertKeyInfo() {
    if (!m_connected) return;
    QString cid = m_kiClientEdit->text().trimmed();
    QString sid = m_kiServerEdit->text().trimmed();
    QString sk = m_kiKeyEdit->text().trimmed();
    if (cid.isEmpty() || sid.isEmpty()) { QMessageBox::warning(this, "提示", "客户端ID和服务端ID不能为空"); return; }
    try {
        char sql[1024];
        snprintf(sql, sizeof(sql), "INSERT INTO SECMNG.SECKEYINFO(clientid,serverid,keyid,createtime,state,seckey) VALUES ('%s','%s',1,SYSDATE,1,'%s')",
                 cid.toStdString().c_str(), sid.toStdString().c_str(), sk.toStdString().c_str());
        auto *s = m_occi->getConnection()->createStatement(sql);
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        m_kiClientEdit->clear(); m_kiServerEdit->clear(); m_kiKeyEdit->clear();
        refreshKeyInfoTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("新增失败:\n") + e.what());
    }
}

void MainWindow::onUpdateKeyInfo() {
    if (!m_connected) return;
    int row = m_keyInfoTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选择行"); return; }
    QString cid = m_keyInfoTable->item(row, 0)->text();
    QString sid = m_keyInfoTable->item(row, 1)->text();
    QString sk = m_kiKeyEdit->text().trimmed();
    if (sk.isEmpty()) { QMessageBox::warning(this, "提示", "请填写新密钥"); return; }
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "UPDATE SECMNG.SECKEYINFO SET seckey='" + sk.toStdString() + "' WHERE clientid='" + cid.toStdString() + "' AND serverid='" + sid.toStdString() + "'");
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        m_kiKeyEdit->clear();
        refreshKeyInfoTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("修改失败:\n") + e.what());
    }
}

void MainWindow::onDeleteKeyInfo() {
    if (!m_connected) return;
    int row = m_keyInfoTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选择行"); return; }
    QString cid = m_keyInfoTable->item(row, 0)->text();
    QString sid = m_keyInfoTable->item(row, 1)->text();
    if (QMessageBox::question(this, "确认", "确定删除该密钥记录吗？") != QMessageBox::Yes) return;
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "DELETE FROM SECMNG.SECKEYINFO WHERE clientid='" + cid.toStdString() + "' AND serverid='" + sid.toStdString() + "'");
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        refreshKeyInfoTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("删除失败:\n") + e.what());
    }
}

// ---- KEYSN ----

void MainWindow::refreshKeysnTable() {
    if (!m_connected) return;
    m_keysnTable->setRowCount(0);
    try {
        auto *s = m_occi->getConnection()->createStatement("SELECT ikeysn FROM SECMNG.KEYSN");
        auto *rs = s->executeQuery();
        while (rs->next()) {
            int r = m_keysnTable->rowCount();
            m_keysnTable->insertRow(r);
            m_keysnTable->setItem(r, 0, new QTableWidgetItem(QString::number(rs->getInt(1))));
        }
        s->closeResultSet(rs);
        m_occi->getConnection()->terminateStatement(s);
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("KEYSN 查询失败:\n") + e.what());
    }
}

void MainWindow::onUpdateKeysn() {
    if (!m_connected) return;
    QString val = m_keysnEdit->text().trimmed();
    if (val.isEmpty()) { QMessageBox::warning(this, "提示", "请输入新序列号"); return; }
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "UPDATE SECMNG.KEYSN SET ikeysn=" + val.toStdString());
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        m_keysnEdit->clear();
        refreshKeysnTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("更新失败:\n") + e.what());
    }
}

// ---- SRVCFG ----

void MainWindow::refreshSrvCfgTable() {
    if (!m_connected) return;
    m_srvCfgTable->setRowCount(0);
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "SELECT key, valude FROM SECMNG.SRVCFG ORDER BY key");
        auto *rs = s->executeQuery();
        while (rs->next()) {
            int r = m_srvCfgTable->rowCount();
            m_srvCfgTable->insertRow(r);
            m_srvCfgTable->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(rs->getString(1))));
            m_srvCfgTable->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(rs->getString(2))));
        }
        s->closeResultSet(rs);
        m_occi->getConnection()->terminateStatement(s);
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("SRVCFG 查询失败:\n") + e.what());
    }
}

void MainWindow::onInsertSrvCfg() {
    if (!m_connected) return;
    QString key = m_srvKeyEdit->text().trimmed();
    QString val = m_srvValEdit->text().trimmed();
    if (key.isEmpty()) { QMessageBox::warning(this, "提示", "键不能为空"); return; }
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "INSERT INTO SECMNG.SRVCFG(key,valude) VALUES ('" + key.toStdString() + "','" + val.toStdString() + "')");
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        m_srvKeyEdit->clear(); m_srvValEdit->clear();
        refreshSrvCfgTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("新增失败:\n") + e.what());
    }
}

void MainWindow::onUpdateSrvCfg() {
    if (!m_connected) return;
    int row = m_srvCfgTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选择行"); return; }
    QString key = m_srvCfgTable->item(row, 0)->text();
    QString val = m_srvValEdit->text().trimmed();
    if (val.isEmpty()) { QMessageBox::warning(this, "提示", "请填写新值"); return; }
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "UPDATE SECMNG.SRVCFG SET valude='" + val.toStdString() + "' WHERE key='" + key.toStdString() + "'");
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        m_srvValEdit->clear();
        refreshSrvCfgTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("修改失败:\n") + e.what());
    }
}

void MainWindow::onDeleteSrvCfg() {
    if (!m_connected) return;
    int row = m_srvCfgTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选择行"); return; }
    QString key = m_srvCfgTable->item(row, 0)->text();
    if (QMessageBox::question(this, "确认", "确定删除配置 " + key + " 吗？") != QMessageBox::Yes) return;
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "DELETE FROM SECMNG.SRVCFG WHERE key='" + key.toStdString() + "'");
        s->setAutoCommit(true);
        s->executeUpdate();
        m_occi->getConnection()->terminateStatement(s);
        refreshSrvCfgTable();
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("删除失败:\n") + e.what());
    }
}

// ---- TRAN ----

void MainWindow::refreshTranTable() {
    if (!m_connected) return;
    m_tranTable->setRowCount(0);
    try {
        auto *s = m_occi->getConnection()->createStatement(
            "SELECT IID,tran_operator,TO_CHAR(trantime,'YYYY-MM-DD HH24:MI'),tranid,trandesc FROM SECMNG.TRAN ORDER BY IID DESC");
        auto *rs = s->executeQuery();
        while (rs->next()) {
            int r = m_tranTable->rowCount();
            m_tranTable->insertRow(r);
            for (int c = 0; c < 5; c++)
                m_tranTable->setItem(r, c, new QTableWidgetItem(QString::fromStdString(rs->getString(c + 1))));
        }
        s->closeResultSet(rs);
        m_occi->getConnection()->terminateStatement(s);
    } catch (oracle::occi::SQLException &e) {
        QMessageBox::critical(this, "错误", QString("TRAN 查询失败:\n") + e.what());
    }
}
