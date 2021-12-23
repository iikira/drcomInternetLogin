#include "drcom_worker.h"
#include "defer.h"
#include "utils.h"
#include "settings.h"
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
    // 加载配置
    networkConfigQuery = SETTINGS->networkConfigQuery();
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
        reply->deleteLater();
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

    // 获取网络配置, 获取urlStr
    // 判断跳转码
    if (code == 302) {
        QString location = reply->header(QNetworkRequest::KnownHeaders::LocationHeader).toString();
        QUrl url(location);
        QUrlQuery query(url);
        QString userIp = query.queryItemValue("UserIP");
        // 如果userIp未获取到, 就访问location来获取ip
        if (userIp.isEmpty()) {
            getUserIp(url, userIp, err);
            if (!err.isEmpty()) {
                return;
            }
        }
        networkConfigQuery = QString("wlanuserip=%1&wlanacip=%2&wlanacname=%3").arg(userIp, "null", "null");
    } else {
        auto body = reply->readAll();
        QRegularExpression exp("<script type=\"text/javascript\">location.href=\"(.*?)\"</script>");
        auto match = exp.match(body);
        if (!match.hasMatch()) {
            err = "未检测到location.href";
            return;
        }

        QString urlStr = match.captured(1);
        QUrl url(urlStr);
        networkConfigQuery = url.query();
    }

    // 保存配置
    SETTINGS->setNetworkConfigQuery(networkConfigQuery);

    // 无错误, err置空
    err.clear();
}

void DrcomWorker::requestLogin() {
    // 发送登录请求
    QNetworkRequest req(QUrl(QString("http://%1/eportal/?c=ACSetting&a=Login&%2").arg(host_, networkConfigQuery)));
    req.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString postData = QString("DDDDD=,0,%1&upass=%2").arg(account_, password_);
    QNetworkReply *reply = netManager->post(req, postData.toUtf8());

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
    QNetworkRequest req(QUrl(QString("http://%1/eportal/?c=ACSetting&a=Logout&%2").arg(host_, networkConfigQuery)));
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
