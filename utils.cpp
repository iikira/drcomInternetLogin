#include "utils.h"
#include <QEventLoop>
#include <QTimer>

void waitNetworkReplyFinish(QNetworkReply *reply) {
    QEventLoop eventLoop;
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
}

void sleep(int secs) {
    QEventLoop eventloop;
    QTimer::singleShot(secs * 1000, &eventloop, SLOT(quit()));
    eventloop.exec();
}
