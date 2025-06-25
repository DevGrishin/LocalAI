#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "AiHandler.h"
#include <QStringList>
#include "ollama.hpp"


int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_CONTROLS_STYLE", "Fusion");
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    MyHandler handler;
    engine.rootContext()->setContextProperty("myHandler", &handler);



    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("LocalAI", "Main");

    QObject *root = engine.rootObjects().first();
    QObject *logColumn = root->findChild<QObject*>("logColumn");
    handler.logColumn = logColumn;

    handler.loadModelsAsync();


    return app.exec();
}







