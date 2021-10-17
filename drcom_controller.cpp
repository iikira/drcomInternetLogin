#include "drcom_controller.h"
#include <QThread>
#include <QtConcurrent>
#include "defer.h"

DrcomController::DrcomController(QObject *parent) :
        QObject(parent), drcomWorker(new DrcomWorker), drcomWorkerThread(new QThread), isKeepLoginRunning(false),
        isWorkerKeepLoginStop(false) {
    drcomWorker->moveToThread(drcomWorkerThread);

    connect(this, &DrcomController::workerLogin, drcomWorker, &DrcomWorker::login);
    connect(this, &DrcomController::workerKeepLogin, drcomWorker, &DrcomWorker::keepLogin);
    connect(this, &DrcomController::workerLogout, drcomWorker, &DrcomWorker::logout);
    connect(drcomWorker, &DrcomWorker::error, this, &DrcomController::error);
    connect(drcomWorker, &DrcomWorker::loginSuccess, this, &DrcomController::workerLoginSuccess);
    connect(drcomWorker, &DrcomWorker::loginDone, this, &DrcomController::workerLoginDone);
    connect(drcomWorker, &DrcomWorker::logoutSuccess, this, &DrcomController::workerLogoutSuccess);
    connect(drcomWorker, &DrcomWorker::logoutDone, this, &DrcomController::workerLogoutDone);

    drcomWorkerThread->start();
}

DrcomController::~DrcomController() {
    connect(drcomWorkerThread, &QThread::finished, drcomWorker, &QThread::deleteLater);
    connect(drcomWorkerThread, &QThread::finished, drcomWorkerThread, &QThread::deleteLater);
    drcomWorkerThread->quit();
}

void DrcomController::login(const QString &host, const QString &account, const QString &password) {
    drcomWorker->setHost(host);
    drcomWorker->setAccount(account);
    drcomWorker->setPassword(password);
    emit workerLogin();
}

void DrcomController::keepLogin(const QString &host, const QString &account, const QString &password) {
    if (isKeepLoginRunning)
        return;
    isKeepLoginRunning = true;
    drcomWorker->setHost(host);
    drcomWorker->setAccount(account);
    drcomWorker->setPassword(password);

    isWorkerKeepLoginStop = false;
    emit workerKeepLogin(&isWorkerKeepLoginStop);
    connect(drcomWorker, &DrcomWorker::keepLoginDone, [&] {
        isKeepLoginRunning = false;
    });
}

void DrcomController::logout(const QString &host, const QString &account) {
    drcomWorker->setHost(host);
    emit workerLogout();
}

void DrcomController::setWorkerKeepLoginStop(bool stop) {
    isWorkerKeepLoginStop = stop;
}
