#pragma once
#include <QTcpServer>
class LocalHttp : public QTcpServer {
    Q_OBJECT
public:
    explicit LocalHttp(QObject* parent=nullptr):QTcpServer(parent){}
    bool start(const QHostAddress& addr, quint16 port){ return listen(addr, port); }
protected:
    void incomingConnection(qintptr handle) override;
};
