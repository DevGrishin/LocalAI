#include "AiHandler.h"
#include "ollama.hpp"
#include <QMetaObject>
#include <QVariant>

QStringList convertToQStringList(const std::vector<std::string>& vec) {
    QStringList list;
    for (const auto& s : vec)
        list << QString::fromStdString(s);
    return list;
}

void MyHandler::loadModelsAsync() {
    std::thread([this]() {
        std::vector<std::string> models = ollama::list_models();
        QStringList list = convertToQStringList(models);
        QMetaObject::invokeMethod(this, [this, list]() {
            setComboOptions(list);  // Safe!
        });
        available = true;
    }).detach();
}

void MyHandler::addItem(const std::string &text) {
    if (!logColumn) {
        qWarning() << "logColumn is not set!";
        return;
    }
    QString qtext = QString::fromStdString(text);
    QMetaObject::invokeMethod(logColumn, "addItem", Q_ARG(QVariant, qtext));
}

void MyHandler::updateLastItem(const std::string &text) {
    QString qtext = QString::fromStdString(text);
    QMetaObject::invokeMethod(logColumn, "updateLastItem", Q_ARG(QVariant, qtext));
}
