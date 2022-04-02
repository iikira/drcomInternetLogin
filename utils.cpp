#include "utils.h"
#include <QEventLoop>
#include <QTimer>
#include <QNetworkInterface>

void waitNetworkReplyFinish(QNetworkReply *reply) {
    QEventLoop eventLoop;
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
}

void sleep(int secs) {
    QEventLoop eventLoop;
    QTimer::singleShot(secs * 1000, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

void timeMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    auto localMsg = msg.toUtf8();
    auto time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8();
    const char *msgType;
    switch (type) {
        case QtDebugMsg:
            msgType = "Debug";
            break;
        case QtInfoMsg:
            msgType = "Info";
            break;
        case QtWarningMsg:
            msgType = "Warning";
            break;
        case QtCriticalMsg:
            msgType = "Critical";
            break;
        case QtFatalMsg:
            msgType = "Fatal";
            break;
        default:
            msgType = QString::number(type).toLocal8Bit().constData();
            break;
    }
    fprintf(stderr, "%s %s: %s\n", time.constData(), msgType, localMsg.constData());
}
