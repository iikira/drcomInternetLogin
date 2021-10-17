#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#define SETTINGS Settings::instance()

class Settings : public QObject {
Q_OBJECT
public:
    Settings();

    static Settings *instance();

    QString networkConfigQuery();

    Q_INVOKABLE QString host();

    Q_INVOKABLE QString account();

    Q_INVOKABLE QString password();

    Q_INVOKABLE bool keepLogin();

    void setNetworkConfigQuery(const QString &value);

    Q_INVOKABLE void setHost(const QString &value);

    Q_INVOKABLE void setAccount(const QString &value);

    Q_INVOKABLE void setPassword(const QString &value);

    Q_INVOKABLE void setKeepLogin(bool value);

private:
    QSettings settings;
};

#endif // SETTINGS_H
