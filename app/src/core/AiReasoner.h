#pragma once
#include <QJsonObject>
#include <QString>
#include "LlamaRunner.h"

class AiReasoner {
public:
    void configure(const QJsonObject& cfg){ cfg_ = cfg; llama_.configure(cfg); }
    bool llmEnabled() const { return llama_.isAvailable(); }
    QString answer(const QString& user);
private:
    QJsonObject cfg_;
    LlamaRunner llama_;
};
