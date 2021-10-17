#pragma once

#include <QNetworkAccessManager>

class DrcomWorker : public QObject {
Q_OBJECT
public:
    explicit DrcomWorker();

    DrcomWorker(const QString &host, const QString &account, const QString &password);

    ~DrcomWorker();

    void setHost(const QString &host);

    void setAccount(const QString &account);

    void setPassword(const QString &password);

public slots:

    void login();

    void keepLogin(bool *stop);

    void logout();

private:
    QString host_;
    QString account_;
    QString password_;

    QNetworkAccessManager *netManager;

    bool isLogin; // 是否已经登录
    QString err; // 错误
    QString networkConfigQuery;

    void checkNetworkState();

    void requestLogin();

signals:

    void error(QString err);

    void loginSuccess();

    void loginDone();

    void keepLoginDone();

    void logoutSuccess();

    void logoutDone();
};
