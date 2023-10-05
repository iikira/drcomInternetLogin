#include "drcom_worker.h"
#include "defer.h"
#include "utils.h"
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QThread>
#include <QUrlQuery>

const QString userAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.81 Safari/537.36";

DrcomWorker::DrcomWorker() :
        netManager(new QNetworkAccessManager(this)) {
    initNetConfig();
}

DrcomWorker::DrcomWorker(const QString &host, const QString &account, const QString &password) :
        host_(host), account_(account), password_(password),
        netManager(new QNetworkAccessManager(this)) {
    initNetConfig();
}

DrcomWorker::~DrcomWorker() {
    netManager->deleteLater();
}

void DrcomWorker::setHost(const QString &host) {
    host_ = host;
}

void DrcomWorker::setAccount(const QString &account) {
    account_ = account;
}

void DrcomWorker::setPassword(const QString &password) {
    password_ = password;
}

void DrcomWorker::initNetConfig() {
    // 禁用proxy
    // netManager 默认不会redirect
    netManager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    netManager->setRedirectPolicy(QNetworkRequest::RedirectPolicy::ManualRedirectPolicy);
}

void DrcomWorker::getUserIp(const QUrl &url, QString &userIp, QString &err) {
    QNetworkReply *reply = netManager->get(QNetworkRequest(url));
    defer([&] {
        reply->deleteLater();
    });
    // 等待响应完毕
    waitNetworkReplyFinish(reply);

    // 检测网络错误
    auto errCode = reply->error();
    if (errCode != QNetworkReply::NoError) {
        err = reply->errorString();
        return;
    }

    auto body = reply->readAll();
    QRegularExpression exp("v46ip='(.*?)'");
    auto match = exp.match(body);
    if (!match.hasMatch()) {
        err = "未检测到userIp";
        return;
    }

    userIp = match.captured(1);
    err.clear(); // 无错误, 清空
}

void DrcomWorker::checkNetworkState(bool &isLogin, QString &err) {
    QNetworkReply *reply = netManager->get(QNetworkRequest(QUrl("http://119.29.29.29")));
    defer([&] {
        delete reply; // 立即释放内存
    });

    // 等待响应完毕
    waitNetworkReplyFinish(reply);

    // 获取http状态码
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int code = statusCode.toInt();
    qDebug() << "request 119.29.29.29, code:" << code;
    isLogin = code == 404;
    if (isLogin) {
        // 网络正常访问
        return;
    }

    // 检测网络的其他错误
    auto errCode = reply->error();
    if (errCode != QNetworkReply::NoError) {
        err = reply->errorString();
        return;
    }

    // 无错误, err置空
    err.clear();
}

void DrcomWorker::requestLogin() {
    // 发送登录请求
    QNetworkRequest req(QUrl(QString("https://%1/eportal/portal/login?callback=dr1003&user_account=,0,%2&user_password=%3").arg(host_, account_, password_)));
    req.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = netManager->get(req);

    waitNetworkReplyFinish(reply);
    reply->deleteLater();
}

void DrcomWorker::login() {
    defer([&] {
        emit loginDone();
    });
    bool isLogin;
    QString err;
    checkNetworkState(isLogin, err);
    if (isLogin) {
        // 已登录
        emit loginSuccess();
        return;
    }
    if (!err.isEmpty()) {
        emit error(err);
        return;
    }

    requestLogin();

    // 检测网络是否连接正常
    checkNetworkState(isLogin, err);
    if (isLogin) {
        // 已登录
        emit loginSuccess();
        return;
    }
    emit error("登录失败");
}

void DrcomWorker::keepLogin(const bool *stop) {
    qDebug() << "keepLogin: running";

    bool isLogin;
    QString err;
    while (!*stop) {
        defer([&] {
            sleep(5);
        });
        checkNetworkState(isLogin, err);
        if (isLogin) {
            // 已登录
            qDebug() << "keepLogin: login success";
            continue;
        }
        if (!err.isEmpty()) {
            qDebug() << "keepLogin err: " << err;
            continue;
        }

        qDebug() << "keepLogin: request login";
        requestLogin();
    }
    qDebug() << "keepLogin: exit";
    emit keepLoginDone();
}

void DrcomWorker::logout() {
    defer([&] {
        emit logoutDone();
    });
    QNetworkRequest req(QUrl(QString("https://%1/eportal/portal/logout?callback=dr1003&login_method=1&user_account=drcom&user_password=123&ac_logout=1&register_mode=1&wlan_user_ipv6=&wlan_vlan_id=0&wlan_user_mac=000000000000&wlan_ac_ip=&wlan_ac_name=&jsVersion=4.1.3&v=8456&lang=zh").arg(host_)));
    req.setHeader(QNetworkRequest::UserAgentHeader, userAgent);

    QNetworkReply *reply = netManager->get(req);

    // 等待响应完毕
    waitNetworkReplyFinish(reply);
    reply->deleteLater();

    // 检测网络是否连接正常
    bool isLogin;
    QString err;
    checkNetworkState(isLogin, err);
    if (!isLogin) {
        // 未登录, 则, 退出登录成功
        emit logoutSuccess();
        return;
    }
    emit error("退出登录失败");
}
