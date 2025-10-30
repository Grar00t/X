#include "ExecSandbox.h"
#include <QProcess>

std::pair<bool, QString> ExecSandbox::runLocal(const QString& cmd, bool dryRun){
    if(dryRun) return {true, "[DRY-RUN] " + cmd};
    QProcess ps;
#ifdef _WIN32
    ps.start("powershell", QStringList{"-NoProfile","-Command", cmd});
#else
    ps.start("bash", QStringList{"-lc", cmd});
#endif
    ps.waitForFinished(60000);
    const QString out = QString::fromUtf8(ps.readAllStandardOutput()) + QString::fromUtf8(ps.readAllStandardError());
    return {ps.exitStatus()==QProcess::NormalExit && ps.exitCode()==0, out};
}

std::pair<bool, QString> ExecSandbox::rebuildLocal(){
    return runLocal("cmake --build build --config Release", /*dryRun=*/false);
}
