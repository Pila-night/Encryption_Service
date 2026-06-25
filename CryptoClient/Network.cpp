#include "Network.h"

Network::Network(QObject *parent)
    : QObject(parent)
    , socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::connected,    this, &Network::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Network::onDisconnected);
    connect(socket, &QTcpSocket::readyRead,    this, &Network::onReadyRead);

    connect(socket, &QTcpSocket::errorOccurred, this, &Network::onError);
}

Network::~Network()
{
}

void Network::connectToServer(const QString &ip, quint16 port)
{
    qDebug() << "  IP:" << ip;
    qDebug() << "  Порт:" << port;
    qDebug() << "  Текущее состояние сокета:" << socket->state();

    if (socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "  Сокет не подключен, устанавливаем соединение...";
        socket->connectToHost(ip, port);
        qDebug() << "  Соединение инициировано";
    } else {
        qDebug() << "  Сокет НЕ в состоянии 'Не подключен' (состояние:" << socket->state() << "), пропускаем подключение";
    }

    qDebug() << "Конец Network::connectToServer";
}
void Network::disconnectFromServer()
{
    if (socket->state() != QAbstractSocket::UnconnectedState) {
        socket->disconnectFromHost();
    }
}

void Network::sendData(const QByteArray &data)
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(data);
        socket->flush();
    }
}

bool Network::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}



void Network::onConnected()
{
    emit connected();
}

void Network::onDisconnected()
{
    emit disconnected();
}

void Network::onReadyRead()
{
    QByteArray data = socket->readAll();
    if (!data.isEmpty()) {
        emit dataReceived(data);
    }
}

void Network::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    QString errorMsg = socket->errorString();
    emit errorOccurred(errorMsg);
}