#ifndef AIHANDLER_H
#define AIHANDLER_H


#include <QObject>
#include <QString>
#include <QDebug>
#include "ollama.hpp"

class MyHandler : public QObject {
    Q_OBJECT
public:
    explicit MyHandler(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    void onEnterPressed(QString text) {
        qDebug() << text;
    }
    Q_INVOKABLE void onComboSelectionChanged(const QString &selected) {
        currModel = selected.toStdString();
    }


private:
    std::string currModel;
};






#endif // AIHANDLER_H
