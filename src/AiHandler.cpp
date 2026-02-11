#include "include/AiHandler.h"
#include "include/ollama.hpp"
#include <QMetaObject>
#include <QVariant>
#include <QDateTime>


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
            setComboOptions(list);
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

void MyHandler::loadConversationHistory() {
    if (!historyList) {
        qWarning() << "historyList is not set!";
        return;
    }

    QMetaObject::invokeMethod(historyList, "clear");
    std::vector<ConversationItem> conversations = getAllConversations();

    for (const auto& conv : conversations) {
        QVariantMap itemData;
        itemData["conversationId"] = conv.id;
        itemData["conversationName"] = conv.name.isEmpty() ? QString("Chat %1").arg(conv.id) : conv.name;
        itemData["isSelected"] = (conv.id == m_currentConversationId);

        QMetaObject::invokeMethod(historyList, "addHistoryItem", Q_ARG(QVariant, itemData));
    }

    emit conversationHistoryChanged();
}

void MyHandler::createNewConversation() {
    m_currentConversationId = -1;
    m_currentConversation = "";
    clearLogColumn();

    loadConversationHistory();

    emit currentConversationChanged();
}

void MyHandler::selectConversation(int id) {
    if (id == m_currentConversationId) return;
    stop = true;

    ConversationItem conv = getConversation(id);
    if (conv.id != -1) {
        m_currentConversationId = id;
        m_currentConversation = conv.conversation.toStdString();

        clearLogColumn();

        populateLogColumn(conv.conversation);
        loadConversationHistory();

        emit currentConversationChanged();
    }
}

void MyHandler::deleteConversation(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM Conversations WHERE id = ?");
    query.bindValue(0, id);

    if (query.exec()) {
        qDebug() << "Conversation deleted successfully";

        if (id == m_currentConversationId) {
            createNewConversation();
        } else {
            loadConversationHistory();
        }
    } else {
        qDebug() << "Error deleting conversation:" << query.lastError().text();
    }
}

void MyHandler::renameConversation(int id, const QString& newName) {
    QSqlQuery query;
    query.prepare("UPDATE Conversations SET name = ? WHERE id = ?");
    query.bindValue(0, newName);
    query.bindValue(1, id);

    if (query.exec()) {
        qDebug() << "Conversation renamed successfully";
        loadConversationHistory();
    } else {
        qDebug() << "Error renaming conversation:" << query.lastError().text();
    }
}
