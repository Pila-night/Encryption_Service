#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "mainwindow.h"
#include "Network.h"
class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(MainWindow *view, QObject *parent = nullptr);
    ~Controller();

private slots:

    /*Слоты обработки действий пользователя в графическом интерфейсе mainwindow*/
    void onConnectedRequested(const QString &ip, const quint16 &port);
    void onDisconnectedRequested();
    void onClearRequested();
    void onPerformRequested(const QString& text, bool isEncrypt);

    /*слоты для обработки сетевых событий  (класса Network)*/
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onNetworkDataReceived(const QByteArray &data);
    void onNetworkError(const QString &text);


private:
    MainWindow *m_view;
    Network *m_network;
};

#endif // CONTROLLER_H
