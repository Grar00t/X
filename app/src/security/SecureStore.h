#pragma once
#include <QByteArray>

struct SecureStore {
    static QByteArray protect(const QByteArray& plain);
    static QByteArray unprotect(const QByteArray& blob);
};
