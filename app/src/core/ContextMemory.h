#pragma once
#include <QString>
#include <QtSql>

class ContextMemory {
public:
    bool openDatabase(const QString& file);
    void appendMessage(const QString& who, const QString& text);
private:
    QSqlDatabase db_;
};
