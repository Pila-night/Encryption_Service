#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>

class Network : public QObject
{
    Q_OBJECT

public:
    explicit Network(QObject *parent = nullptr);
    ~Network();

    void connectToServer(const QString &ip, quint16 port);
    void disconnectFromServer();
    void sendData(const QByteArray &data);

    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &errorMessage);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
};

#endif // NETWORK_H