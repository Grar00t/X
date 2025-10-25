#pragma once
#include <QMainWindow>
#include <QJsonObject>
class QTextEdit; class QLineEdit; class QPushButton; class QCheckBox; class QLabel;
class QTabWidget; class QTreeWidget; class QComboBox; class QTimer;

#include "../core/AiReasoner.h"
#include "../core/ExecSandbox.h"
#include "../core/ContextMemory.h"
#include "../api/LocalHttp.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent=nullptr);
    void loadConfig(const QString& tomlPath);

private slots:
    void onSend();
    void onRunLocal();
    void onSSHAction();
    void onToggleSovereign(bool checked);
    void onGithubLogin();
    void onRebuildLocal();
    void onRebuildSSH();

private:
    void setupUi();
    void appendChat(const QString& who, const QString& text);
    void refreshStatus();
    void applyModeUi();

private:
    // UI
    QTextEdit* chatView_{};
    QLineEdit* chatInput_{};
    QPushButton* sendBtn_{};
    QCheckBox* sovereignChk_{};
    QPushButton* githubBtn_{};
    QLabel* statusLbl_{};
    QTabWidget* tabs_{};
    QTextEdit* logs_{};
    QLineEdit* cmdLine_{};
    QPushButton* runLocalBtn_{};
    QPushButton* runSSHBtn_{};
    QPushButton* rebuildLocalBtn_{};
    QPushButton* rebuildSSHBtn_{};
    QTreeWidget* leftNav_{};
    QComboBox* langCmb_{};
    // Core
    AiReasoner reasoner_;
    ExecSandbox sandbox_;
    ContextMemory memory_;
    LocalHttp api_;
    // Config/State
    QJsonObject cfg_;
    QTimer* statusTimer_{};
};
