#pragma once

#include <QNetworkAccessManager>

class DrcomWorker : public QObject {
Q_OBJECT
public:
    explicit DrcomWorker();

    DrcomWorker(const QString &host, const QString &account, const QString &password);

    ~DrcomWorker() override;

    void setHost(const QString &host);

    void setAccount(const QString &account);

    void setPassword(const QString &password);

public slots:

    void login();

    void keepLogin(const bool *stop);

    void logout();

private:
    QString host_;
    QString account_;
    QString password_;

    QNetworkAccessManager *netManager;

    QString networkConfigQuery;

    void checkNetworkState(bool &isLogin, QString &err);

    void requestLogin();

signals:

    void error(QString err);

    void loginSuccess();

    void loginDone();

    void keepLoginDone();

    void logoutSuccess();

    void logoutDone();
};
