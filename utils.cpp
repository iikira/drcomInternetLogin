#include "utils.h"
#include <QEventLoop>
#include <QTimer>

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
