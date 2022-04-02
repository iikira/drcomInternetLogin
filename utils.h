#ifndef UTILS_H
#define UTILS_H

#include <QNetworkReply>

void waitNetworkReplyFinish(QNetworkReply *reply);

void sleep(int secs);

void timeMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // UTILS_H
