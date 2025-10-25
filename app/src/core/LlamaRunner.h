#pragma once
#include <QJsonObject>
#include <QString>

class LlamaRunner {
public:
    void configure(const QJsonObject& cfg);
    bool isAvailable() const { return available_; }
    QString infer(const QString& system, const QString& prompt, double temp, double top_p);
private:
    bool available_ = false;
    QString exePath_;
    QString modelPath_;
    int ctx_ = 4096;
};
