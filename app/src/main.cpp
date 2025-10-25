#include <QApplication>
#include <QCommandLineParser>
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("GrA — Sovereign Intelligence OS");
    QApplication::setOrganizationName("GrA");

    QCommandLineParser cli;
    cli.setApplicationDescription("GrA — Zero-Cloud Sovereign Intelligence");
    cli.addHelpOption();
    QCommandLineOption cfgOpt({"c","config"}, "Path to settings.toml", "file", "config/settings.toml");
    cli.addOption(cfgOpt);
    cli.process(app);

    MainWindow w;
    w.loadConfig(cli.value(cfgOpt));
    w.show();
    return app.exec();
}
