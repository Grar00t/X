#include "MainWindow.h"
#include <QtWidgets>
#include <QtNetwork>
#include <QtSql>

static QJsonObject loadTomlAsJson(const QString& path){
    QJsonObject o;
    QFile f(path);
    if(f.open(QIODevice::ReadOnly)){
        const QString t = QString::fromUtf8(f.readAll());
        o["config_path"] = path;
        o["mode_sovereign"] = t.contains("sovereign = true");
        o["llm_enabled"] = t.contains("use_llama = true");
        o["github_enabled"] = t.contains("[github]") && t.contains("enabled = true") && !t.contains("sovereign = true");
    }
    return o;
}

MainWindow::MainWindow(QWidget* parent):QMainWindow(parent){
    setupUi();
    statusTimer_ = new QTimer(this);
    connect(statusTimer_, &QTimer::timeout, this, &MainWindow::refreshStatus);
    statusTimer_->start(1000);
    api_.start(QHostAddress("127.0.0.1"), 43126); // /health, POST /v1/chat (stub)
}

void MainWindow::loadConfig(const QString& tomlPath){
    cfg_ = loadTomlAsJson(tomlPath);
    reasoner_.configure(cfg_);
    memory_.openDatabase("gra_brain.db");
    appendChat("system", "مرحبًا بك في GrA — الوضع السيادي مفعل افتراضيًا.");
    applyModeUi();
}

void MainWindow::setupUi(){
    setWindowTitle("GrA — Sovereign Intelligence OS");
    resize(1200, 740);

    leftNav_ = new QTreeWidget; leftNav_->setHeaderLabel("Panels");
    leftNav_->addTopLevelItem(new QTreeWidgetItem(QStringList()<<"Chat"));
    leftNav_->addTopLevelItem(new QTreeWidgetItem(QStringList()<<"Console"));
    leftNav_->addTopLevelItem(new QTreeWidgetItem(QStringList()<<"Memory"));
    leftNav_->addTopLevelItem(new QTreeWidgetItem(QStringList()<<"Settings"));

    chatView_ = new QTextEdit; chatView_->setReadOnly(true);
    chatInput_ = new QLineEdit;
    sendBtn_ = new QPushButton("Send");
    connect(sendBtn_, &QPushButton::clicked, this, &MainWindow::onSend);

    cmdLine_ = new QLineEdit; cmdLine_->setPlaceholderText("PowerShell / CMD...");
    runLocalBtn_ = new QPushButton("Run Local");
    runSSHBtn_   = new QPushButton("Run SSH");
    connect(runLocalBtn_, &QPushButton::clicked, this, &MainWindow::onRunLocal);
    connect(runSSHBtn_,   &QPushButton::clicked, this, &MainWindow::onSSHAction);

    rebuildLocalBtn_ = new QPushButton("Rebuild (Local)");
    rebuildSSHBtn_   = new QPushButton("Rebuild (SSH)");
    connect(rebuildLocalBtn_, &QPushButton::clicked, this, &MainWindow::onRebuildLocal);
    connect(rebuildSSHBtn_,   &QPushButton::clicked, this, &MainWindow::onRebuildSSH);

    tabs_ = new QTabWidget;
    QWidget* chatTab = new QWidget;
    {
        auto lay = new QVBoxLayout(chatTab);
        lay->addWidget(chatView_);
        auto row = new QHBoxLayout;
        row->addWidget(chatInput_);
        row->addWidget(sendBtn_);
        lay->addLayout(row);
    }
    QWidget* consoleTab = new QWidget;
    {
        auto lay = new QVBoxLayout(consoleTab);
        lay->addWidget(cmdLine_);
        auto row = new QHBoxLayout;
        row->addWidget(runLocalBtn_);
        row->addWidget(runSSHBtn_);
        row->addWidget(rebuildLocalBtn_);
        row->addWidget(rebuildSSHBtn_);
        lay->addLayout(row);
        logs_ = new QTextEdit; logs_->setReadOnly(true);
        lay->addWidget(logs_);
    }
    QWidget* settingsTab = new QWidget;
    {
        auto lay = new QVBoxLayout(settingsTab);
        sovereignChk_ = new QCheckBox("Sovereign (Zero-Cloud)");
        sovereignChk_->setChecked(true);
        connect(sovereignChk_, &QCheckBox::toggled, this, &MainWindow::onToggleSovereign);
        langCmb_ = new QComboBox; langCmb_->addItems({"auto","ar","en"});
        githubBtn_ = new QPushButton("Sign in with GitHub");
        connect(githubBtn_, &QPushButton::clicked, this, &MainWindow::onGithubLogin);
        lay->addWidget(sovereignChk_);
        lay->addWidget(new QLabel("UI Language:"));
        lay->addWidget(langCmb_);
        lay->addWidget(githubBtn_);
        lay->addStretch();
    }
    tabs_->addTab(chatTab, "Chat");
    tabs_->addTab(consoleTab, "Console");
    tabs_->addTab(new QWidget, "Memory");
    tabs_->addTab(settingsTab, "Settings");

    auto split = new QSplitter;
    split->addWidget(leftNav_);
    split->addWidget(tabs_);
    split->setStretchFactor(1, 1);
    setCentralWidget(split);

    statusLbl_ = new QLabel("Ready");
    statusBar()->addPermanentWidget(statusLbl_);
}

void MainWindow::applyModeUi(){
    const bool sovereign = cfg_.value("mode_sovereign").toBool(true);
    sovereignChk_->setChecked(sovereign);
    githubBtn_->setEnabled(!sovereign && cfg_.value("github_enabled").toBool(false));
}

void MainWindow::appendChat(const QString& who, const QString& text){
    const QString t = QString("[%1] %2: %3")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(who, text.toHtmlEscaped());
    chatView_->append(t);
    memory_.appendMessage(who, text);
}

void MainWindow::refreshStatus(){
    statusLbl_->setText(QString("Sovereign:%1 | LLM:%2 | API: http://127.0.0.1:43126")
        .arg(sovereignChk_->isChecked()?"ON":"OFF")
        .arg(reasoner_.llmEnabled()?"ON":"OFF"));
}

void MainWindow::onSend(){
    const QString msg = chatInput_->text().trimmed();
    if(msg.isEmpty()) return;
    appendChat("you", msg);
    chatInput_->clear();
    const auto reply = reasoner_.answer(msg);
    appendChat("GrA", reply);
}

void MainWindow::onRunLocal(){
    const QString cmd = cmdLine_->text().trimmed();
    if(cmd.isEmpty()) return;
    auto [ok, out] = sandbox_.runLocal(cmd, /*dryRun=*/false);
    logs_->append(ok ? QString("✅ LOCAL OK:\n%1").arg(out) : QString("❌ LOCAL ERR:\n%1").arg(out));
}

void MainWindow::onSSHAction(){
#ifdef GRA_HAS_LIBSSH2
    const QString cmd = cmdLine_->text().trimmed();
    if(cmd.isEmpty()) return;
    auto [ok, out] = sandbox_.runSSH("127.0.0.1", 22, "user", "C:/keys/id_rsa", cmd, /*dryRun=*/true);
    logs_->append(ok ? QString("🛰️ SSH DRY-RUN OK:\n%1").arg(out) : QString("❌ SSH ERR:\n%1").arg(out));
#else
    QMessageBox::warning(this, "SSH", "libssh2 غير مفعّل/مثبّت. عطّل GRA_ENABLE_SSH=OFF أو ثبّت libssh2.");
#endif
}

void MainWindow::onToggleSovereign(bool checked){
    cfg_["mode_sovereign"] = checked;
    applyModeUi();
    appendChat("system", checked ? "الوضع السيادي: فعال" : "الوضع السيادي: متصل (اختياري)");
}

void MainWindow::onGithubLogin(){
    if(sovereignChk_->isChecked()){
        QMessageBox::information(this, "GitHub", "الوضع السيادي مفعّل—الدخول معطّل.");
        return;
    }
    appendChat("system", "GitHub OAuth: فتح المتصفح … (stub)");
    QDesktopServices::openUrl(QUrl("https://github.com/login"));
}

void MainWindow::onRebuildLocal(){
    auto [ok, out] = sandbox_.rebuildLocal();
    logs_->append(ok ? QString("🔁 Rebuild Local OK:\n%1").arg(out) : QString("❌ Rebuild Local ERR:\n%1").arg(out));
}

void MainWindow::onRebuildSSH(){
#ifdef GRA_HAS_LIBSSH2
    auto [ok, out] = sandbox_.rebuildSSH("127.0.0.1", 22, "user", "C:/keys/id_rsa");
    logs_->append(ok ? QString("🔁 Rebuild SSH OK:\n%1").arg(out) : QString("❌ Rebuild SSH ERR:\n%1").arg(out));
#else
    logs_->append("SSH rebuild disabled (no libssh2).");
#endif
}
