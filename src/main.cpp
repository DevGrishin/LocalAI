#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "include/AiHandler.h"
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/libs");
    qputenv("QT_QUICK_CONTROLS_STYLE", "Fusion");
    QGuiApplication app(argc, argv);
    qmlRegisterType<ConversationItem>("ConversationItem", 1, 0, "ConversationItem");

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
    QObject *historyList = root->findChild<QObject*>("historyList");

    handler.logColumn = logColumn;
    handler.historyList = historyList;

    QSqlDatabase m_db;
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("Conversations.db");

    if (!m_db.open()) {
        qDebug() << "Error: connection with database fail";
    } else {
        qDebug() << "Database: connection ok";
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS Conversations (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL DEFAULT '', conversation TEXT NOT NULL DEFAULT '');");

    handler.loadModelsAsync();
    handler.loadConversationHistory();

    return app.exec();
}
