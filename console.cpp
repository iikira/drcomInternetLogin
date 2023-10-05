#include <QCoreApplication>
#include <QCommandLineParser>
#include "utils.h"
#include "drcom_controller.h"

int main(int argc, char *argv[]) {
    qInstallMessageHandler(timeMessageHandler);
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("drcomInternetLoginCore");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Description: This is example of console application");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption hostOption(QStringList() << "o" << "host", "host"
                                ,"host", "p.njupt.edu.cn:802");
    QCommandLineOption accountOption(QStringList() << "a" << "account", "account"
                                ,"account", "account");
    QCommandLineOption passwordOption(QStringList() << "p" << "password", "password"
                                ,"password", "password");

    parser.addOptions(QList<QCommandLineOption>() << hostOption << accountOption << passwordOption);
    parser.process(app);

    QString host = parser.value(hostOption);
    QString account = parser.value(accountOption);
    QString password = parser.value(passwordOption);

    DrcomController drcomController;
    drcomController.keepLogin(host, account, password);
    QObject::connect(&drcomController, &DrcomController::error, [&](const QString& err){
        qInfo() << "err:" << err;
    });

    return QCoreApplication::exec();
}