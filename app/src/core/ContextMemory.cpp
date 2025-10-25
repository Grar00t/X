#include "ContextMemory.h"

bool ContextMemory::openDatabase(const QString& file){
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(file);
    if(!db_.open()) return false;
    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS chatlog(ts TEXT, role TEXT, text TEXT)");
    return true;
}

void ContextMemory::appendMessage(const QString& who, const QString& text){
    if(!db_.isOpen()) return;
    QSqlQuery q;
    q.prepare("INSERT INTO chatlog(ts, role, text) VALUES(datetime('now'), ?, ?)");
    q.addBindValue(who);
    q.addBindValue(text);
    q.exec();
}
