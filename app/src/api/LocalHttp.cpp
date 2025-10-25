#include "LocalHttp.h"
#include <QTcpSocket>
#include <QDateTime>

void LocalHttp::incomingConnection(qintptr handle){
    auto* s = new QTcpSocket(this);
    s->setSocketDescriptor(handle);
    connect(s, &QTcpSocket::readyRead, this, [s]{
        const QByteArray req = s->readAll();
        const bool isHealth = req.startsWith("GET /health");
        const bool isChat = req.startsWith("POST /v1/chat");

        QByteArray body;
        if(isHealth){
            body = QByteArray("{\"ok\":true,\"ts\":\"")
                + QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toUtf8() + "\"}";
        } else if(isChat){
            body = R"({"status":"use UI chat"})";
        } else {
            body = R"({"err":"not found"})";
        }

        QByteArray resp = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                          + QByteArray::number(body.size()) + "\r\n\r\n" + body;
        s->write(resp);
        s->disconnectFromHost();
    });
    connect(s, &QTcpSocket::disconnected, s, &QObject::deleteLater);
}
