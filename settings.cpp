#include "settings.h"
#include <QObject>

Q_GLOBAL_STATIC(Settings, settingsInstance)

Settings::Settings() {

}

Settings *Settings::instance() {
    return settingsInstance();
}

QString Settings::networkConfigQuery() {
    return std::move(settings.value("networkConfigQuery").toString());
}

QString Settings::host() {
    return std::move(settings.value("host").toString());
}

QString Settings::account() {
    return std::move(settings.value("account").toString());
}

QString Settings::password() {
    return std::move(settings.value("password").toString());
}

bool Settings::keepLogin() {
    return settings.value("keepLogin").toBool();
}

void Settings::setNetworkConfigQuery(const QString &value) {
    settings.setValue("networkConfigQuery", value);
}

void Settings::setHost(const QString &value) {
    settings.setValue("host", value);
}

void Settings::setAccount(const QString &value) {
    settings.setValue("account", value);
}

void Settings::setPassword(const QString &value) {
    settings.setValue("password", value);
}

void Settings::setKeepLogin(bool value) {
    settings.setValue("keepLogin", value);
}
