#pragma once

#include "drcom_worker.h"

class DrcomController : public QObject {
Q_OBJECT
public:
    explicit DrcomController(QObject *parent = nullptr);

    ~DrcomController();

    Q_INVOKABLE void login(const QString &host, const QString &account, const QString &password);

    Q_INVOKABLE void keepLogin(const QString &host, const QString &account, const QString &password);

    Q_INVOKABLE void logout(const QString &host, const QString &account);

    Q_INVOKABLE void setWorkerKeepLoginStop(bool stop);

private:
    DrcomWorker *drcomWorker;
    QThread *drcomWorkerThread;

    bool isKeepLoginRunning;
    bool isWorkerKeepLoginStop;

signals:

    void workerLogin();

    void workerKeepLogin(bool *stop);

    void workerLogout();

    void workerError(QString err);

    void workerLoginSuccess();

    void workerLoginDone();

    void workerLogoutSuccess();

    void workerLogoutDone();

    void error(QString err);
};
