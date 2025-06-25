#ifndef AIHANDLER_H
#define AIHANDLER_H


#include <QObject>
#include <QString>
#include <QDebug>
#include "ollama.hpp"



class MyHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList comboOptions READ comboOptions NOTIFY comboOptionsChanged)
    Q_PROPERTY(bool available READ isAvailable CONSTANT)
public:
    using QObject::QObject;

    explicit MyHandler(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void loadModelsAsync();


    bool isAvailable() const { return available; }

    Q_INVOKABLE void onComboSelectionChanged(const QString &selected) {
        currModel = selected.toStdString();
    }

    QStringList comboOptions() const {
        return m_comboOptions;
    }
    QObject *logColumn;


    Q_INVOKABLE void addItem(const std::string &text);
    Q_INVOKABLE void updateLastItem(const std::string &text);

    std::string markdownToHtml(const std::string &md) {
        QString html = QString::fromStdString(md);
        html.replace("**", "<b>").replace("<b>", "</b>", Qt::CaseSensitive);
        html.replace("*", "<i>").replace("<i>", "</i>", Qt::CaseSensitive);

        // Replace line breaks with HTML <br>
        html.replace("\n", "<br>");
        return html.toStdString();
    }



public slots:
    void onEnterPressed(QString text) {
        available = false;
        std::string userText = text.toStdString();
        aiText = "AI:\n";
        done = false;

        addItem("User:\n" + userText);
        addItem(""); // Placeholder for AI text

        std::thread([this, userText]() {
            auto callback = [this](const ollama::response& response) {
                return this->on_receive_response(response);
            };

            ollama::generate("llama3:8b", userText, callback);
            available = true;

            // Optionally emit signal on finish
        }).detach();
    }


signals:
    void modelsReady(QStringList list);
    void comboOptionsChanged();
    void messageAdded(QString message);

private:
    bool available = false;
    std::string currModel;
    QStringList m_comboOptions;
    std::string aiText = "";
    std::atomic<bool> done{false};

    void setComboOptions(const QStringList &list) {
        if (list != m_comboOptions) {
            m_comboOptions = list;
            emit comboOptionsChanged();  // <- This makes the QML ComboBox update
        }
    }
    bool on_receive_response(const ollama::response& response) {
        aiText += std::string(response);
        aiText = markdownToHtml(aiText);
        qDebug() << std::string(response);
        // Emit update to main thread (async-safe)
        QMetaObject::invokeMethod(this, [this]() {
            updateLastItem(aiText);  // your existing method
        }, Qt::QueuedConnection);

        if (response.as_json().contains("done") && response.as_json()["done"] == true) {
            done = true;
        }

        return !done;
    }


};









#endif // AIHANDLER_H
