#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "AiHandler.h"
#include <QStringList>
#include <vector>
#include <string>
#include "ollama.hpp"

QStringList convertToQStringList(const std::vector<std::string>& vec) {
    QStringList list;
    for (const auto& s : vec) {
        list << QString::fromStdString(s);
    }
    return list;
}


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("LocalAI", "Main");

    MyHandler handler;
    engine.rootContext()->setContextProperty("myHandler", &handler);

    std::vector<std::string> myOptions = ollama::list_models();
    QStringList comboOptions = convertToQStringList(myOptions);
    engine.rootContext()->setContextProperty("comboOptions", comboOptions);
    return app.exec();
}



