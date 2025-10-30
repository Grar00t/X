#pragma once
#include <QString>
#include <utility>

class ExecSandbox {
public:
    std::pair<bool, QString> runLocal(const QString& cmd, bool dryRun);
    std::pair<bool, QString> rebuildLocal();
#ifdef GRA_HAS_LIBSSH2
    std::pair<bool, QString> runSSH(const QString& host,int port,const QString& user,const QString& key, const QString& cmd,bool dryRun);
    std::pair<bool, QString> rebuildSSH(const QString& host,int port,const QString& user,const QString& key);
#else
    std::pair<bool, QString> runSSH(...) { return {false, "SSH disabled"}; }
    std::pair<bool, QString> rebuildSSH(...) { return {false, "SSH disabled"}; }
#endif
};
