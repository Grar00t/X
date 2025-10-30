#include "LlamaRunner.h"
#include <QFile>
#include <QProcess>

void LlamaRunner::configure(const QJsonObject& cfg){
    available_ = false;
    const bool use = cfg.value("llm_enabled").toBool(false);
    exePath_ = cfg.value("llama_cpp_exe").toString();
    modelPath_ = cfg.value("model_path").toString();
    ctx_ = 4096;
    if(use && QFile::exists(exePath_) && QFile::exists(modelPath_)) available_ = true;
}

QString LlamaRunner::infer(const QString& system, const QString& prompt, double temp, double top_p){
    if(!available_) return {};
    QStringList args{
        "-m", modelPath_,
        "--ctx-size", QString::number(ctx_),
        "--temp", QString::number(temp),
        "--top-p", QString::number(top_p),
        "-n", "512",
        "-p", system + "\n\nUser: " + prompt + "\nAssistant:"
    };
    QProcess p;
    p.start(exePath_, args);
    p.waitForStarted(15000);
    p.waitForFinished(120000);
    auto out = QString::fromUtf8(p.readAllStandardOutput());
    if(out.isEmpty()){
        auto err = QString::fromUtf8(p.readAllStandardError());
        return err.isEmpty() ? QString() : ("LLM ERR:\n"+err.left(1000));
    }
    return out.left(2000);
}
