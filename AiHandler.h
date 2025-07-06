#ifndef AIHANDLER_H
#define AIHANDLER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "ollama.hpp"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QQmlListProperty>
#include <QDateTime>
#include <atomic>


struct ConversationItem {
    int id;
    QString name;
    QString conversation;

    ConversationItem(int id = -1, const QString& name = "", const QString& conversation = "")
        : id(id), name(name), conversation(conversation) {}
};

Q_DECLARE_METATYPE(ConversationItem)

class MyHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList comboOptions READ comboOptions NOTIFY comboOptionsChanged)
    Q_PROPERTY(bool available READ isAvailable CONSTANT)
    Q_PROPERTY(bool stop READ isStop WRITE setStop CONSTANT)
    Q_PROPERTY(int currentConversationId READ currentConversationId NOTIFY currentConversationChanged)

public:
    using QObject::QObject;
    explicit MyHandler(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void loadModelsAsync();
    Q_INVOKABLE void loadConversationHistory();
    Q_INVOKABLE void createNewConversation();
    Q_INVOKABLE void selectConversation(int id);
    Q_INVOKABLE void deleteConversation(int id);
    Q_INVOKABLE void renameConversation(int id, const QString& newName);

    bool isAvailable() const { return available; }
    bool isStop() const { return stop; }
    void setStop(bool val) { stop = true; }
    int currentConversationId() const { return m_currentConversationId; }

    Q_INVOKABLE void onComboSelectionChanged(const QString &selected) {
        currModel = selected.toStdString();
    }

    QStringList comboOptions() const {
        return m_comboOptions;
    }

    QObject *logColumn;
    QObject *historyList;

    Q_INVOKABLE void addItem(const std::string &text);
    Q_INVOKABLE void updateLastItem(const std::string &text);

    std::string markdownToHtml(const std::string &md) {
        QString html = QString::fromStdString(md);
        html.replace("**", "<b>").replace("<b>", "</b>", Qt::CaseSensitive);
        html.replace("*", "<i>").replace("<i>", "</i>", Qt::CaseSensitive);
        html.replace("\n", "<br>");
        return html.toStdString();
    }

    int saveNewConvo(const std::string &name, const std::string &conversation) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "thread_connection");
        db.setDatabaseName("Conversations.db");
        if (!db.open()) {
            qDebug() << "Failed to open DB:" << db.lastError().text();
            return -1;
        }

        QSqlQuery query(db);
        if (!query.prepare("INSERT INTO Conversations (name, conversation) VALUES (:name, :conversation)")) {
            qDebug() << "Prepare failed:" << query.lastError().text();
            return -1;
        }
        query.bindValue(":name", QString::fromStdString(name));
        query.bindValue(":conversation", QString::fromStdString(conversation));
        qDebug() << "Query isValid:" << query.isValid();

        if (!query.exec()) {
            qDebug() << "Error inserting conversation:" << query.lastError().text();
            return -1;
        } else {
            int newId = query.lastInsertId().toInt();
            qDebug() << "Conversation inserted successfully with ID:" << newId;

            QMetaObject::invokeMethod(this, [this]() {
                loadConversationHistory();
            }, Qt::QueuedConnection);

            return newId;
        }
    }

    void updateConvo(int id, const std::string &conversation) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "update_thread_connection");
        db.setDatabaseName("Conversations.db");
        if (!db.open()) {
            qDebug() << "Failed to open DB for update:" << db.lastError().text();
            return;
        }

        QSqlQuery query(db);
        query.prepare("UPDATE Conversations SET conversation = ? WHERE id = ?");
        query.bindValue(0, QString::fromStdString(conversation));
        query.bindValue(1, id);

        if (!query.exec()) {
            qDebug() << "Error updating conversation:" << query.lastError().text();
        } else {
            qDebug() << "Conversation updated successfully for ID:" << id;
        }

        db.close();
    }

    void updateConvoName(int id, const std::string &name) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "update_name_thread_connection");
        db.setDatabaseName("Conversations.db");
        if (!db.open()) {
            qDebug() << "Failed to open DB for name update:" << db.lastError().text();
            return;
        }

        QSqlQuery query(db);
        query.prepare("UPDATE Conversations SET name = ? WHERE id = ?");
        query.bindValue(0, QString::fromStdString(name));
        query.bindValue(1, id);

        if (!query.exec()) {
            qDebug() << "Error updating conversation name:" << query.lastError().text();
        }

        db.close();
    }

    ConversationItem getConversation(int id) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "get_thread_connection");
        db.setDatabaseName("Conversations.db");
        if (!db.open()) {
            qDebug() << "Failed to open DB for get:" << db.lastError().text();
            return ConversationItem();
        }

        QSqlQuery query(db);
        query.prepare("SELECT id, name, conversation FROM Conversations WHERE id = ?");
        query.bindValue(0, id);

        ConversationItem result;
        if (query.exec() && query.next()) {
            result = ConversationItem(
                query.value(0).toInt(),
                query.value(1).toString(),
                query.value(2).toString()
                );
        }

        db.close();
        return result;
    }

    std::vector<ConversationItem> getAllConversations() {
        std::vector<ConversationItem> conversations;
        QSqlQuery query("SELECT id, name, conversation FROM Conversations ORDER BY id DESC");

        while (query.next()) {
            conversations.push_back(ConversationItem(
                query.value(0).toInt(),
                query.value(1).toString(),
                query.value(2).toString()
                ));
        }
        return conversations;
    }

public slots:
    void onEnterPressed(QString text) {
        available = false;
        stop = false;
        std::string userText = text.toStdString();
        aiText = "AI:\n";
        done = false;
        int generationConversationId = m_currentConversationId;
        std::string generationConversation = m_currentConversation;


        generationConversation += "User:\n" + userText + "\n\n";

        if (generationConversationId == -1) {
            QString defaultName = QString("Chat %1").arg(QDateTime::currentSecsSinceEpoch());
            generationConversationId = saveNewConvo(defaultName.toStdString(), generationConversation);

            m_currentConversationId = generationConversationId;
            m_currentConversation = generationConversation;
            emit currentConversationChanged();
        } else {
            m_currentConversation = generationConversation;
            updateConvo(generationConversationId, generationConversation);
        }

        std::string fullPrompt = generationConversation;
        fullPrompt += "AI:\n";

        addItem("User:\n" + userText);
        addItem("");

        std::thread([this, userText, fullPrompt, generationConversationId, generationConversation]() {
            auto callback = [this, generationConversationId, generationConversation](const ollama::response& response) {
                return this->on_receive_response(response, generationConversationId, generationConversation);
            };

            ollama::generate(currModel, fullPrompt, callback);
            QMetaObject::invokeMethod(this, [this]() {
                available = true;
            }, Qt::QueuedConnection);
        }).detach();
    }

signals:
    void modelsReady(QStringList list);
    void comboOptionsChanged();
    void messageAdded(QString message);
    void currentConversationChanged();
    void conversationHistoryChanged();

private:
    bool available = false;
    bool stop = false;
    std::string currModel;
    QStringList m_comboOptions = QStringList();
    std::string aiText = "";
    std::atomic<bool> done{false};
    int m_currentConversationId = -1;
    std::string m_currentConversation = "";

    void setComboOptions(const QStringList &list) {
        if (list != m_comboOptions) {
            m_comboOptions = list;
            emit comboOptionsChanged();
        }
    }

    bool on_receive_response(const ollama::response& response, int generationConversationId, std::string generationConversation) {
        aiText += std::string(response);

        if (generationConversationId == m_currentConversationId) {
            QMetaObject::invokeMethod(this, [this]() {
                updateLastItem(aiText);
            }, Qt::QueuedConnection);
        }

        // static int responseCount = 0;
        // responseCount++;
        // if (responseCount % 50 == 0) {
        //     ConversationItem currentConvo = getConversation(generationConversationId);
        //     std::string updatedConversation = currentConvo.conversation.toStdString();
        //     updatedConversation += aiText + "\n\n";
        //     updateConvo(generationConversationId, updatedConversation);
        //
        //     if (generationConversationId == m_currentConversationId) {
        //         m_currentConversation = updatedConversation;
        //     }
        // }

        if (response.as_json().contains("done") && response.as_json()["done"] == true) {
            aiText = markdownToHtml(aiText);

            if (generationConversationId == m_currentConversationId) {
                QMetaObject::invokeMethod(this, [this]() {
                    updateLastItem(aiText);
                }, Qt::QueuedConnection);
            }

            ConversationItem currentConvo = getConversation(generationConversationId);
            std::string updatedConversation = currentConvo.conversation.toStdString();

            updatedConversation += aiText + "\n\n";
            updateConvo(generationConversationId, updatedConversation);


            if (generationConversationId == m_currentConversationId) {
                m_currentConversation = updatedConversation;
            }

            QMetaObject::invokeMethod(this, [this]() {
                loadConversationHistory();
            }, Qt::QueuedConnection);

            done = true;
            // responseCount = 0;
        }

        if (stop) {
            return false;
        }

        return !done;
    }

    void clearLogColumn() {
        if (logColumn) {
            QMetaObject::invokeMethod(logColumn, "clear");
        }
    }

    void populateLogColumn(const QString& conversation) {
        if (logColumn && !conversation.isEmpty()) {
            QStringList messages = conversation.split("\n\n", Qt::SkipEmptyParts);
            for (const QString& message : messages) {
                if (!message.trimmed().isEmpty()) {
                    QMetaObject::invokeMethod(logColumn, "addItem", Q_ARG(QVariant, message.trimmed()));
                }
            }
        }
    }
};

#endif
