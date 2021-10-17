#include <QApplication>
#include <QQmlApplicationEngine>
#include <QThread>
#include <QQmlContext>
#include "drcom_controller.h"
#include "settings.h"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QCoreApplication::setOrganizationName("iikira");
    QCoreApplication::setOrganizationDomain("iikira.com");
    QCoreApplication::setApplicationName("drcomInternetLogin");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            }, Qt::QueuedConnection);

    auto drcomController = new DrcomController;
    auto context = engine.rootContext();
    context->setContextProperty("drcomController", drcomController);
    context->setContextProperty("settings", SETTINGS);

    engine.load(url);
    return app.exec();
}
