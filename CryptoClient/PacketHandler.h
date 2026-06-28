#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <QObject>
#include <QByteArray>
#include "ProtocolPackets.h"
#include "ProtocolSerializer.h"
#include "ProtocolAdapter.h"

class PacketHandler : public QObject
{
    Q_OBJECT

public:
    explicit PacketHandler(QObject *parent = nullptr);
    ~PacketHandler();

    void feedData(const QByteArray &data);

    void reset();

signals:
    void publicKeyReceived(const QByteArray &key);
    void nonceReceived(const QByteArray &nonce);
    void signatureReceived(const QByteArray &signature);
    void ciphertextReceived(const QByteArray &payload);
    void plaintextReceived(const QByteArray &payload);
    void disconnectReceived();
    void errorOccurred(const QString &errorMessage);

private:
    QByteArray m_buffer;


    bool tryProcessOnePacket();


    int expectedPacketSize(const QByteArray &data);

    void handlePublicKeyPacket(const QByteArray &packetData);
    void handleNoncePacket(const QByteArray &packetData);
    void handleSignaturePacket(const QByteArray &packetData);
    void handleCiphertextPacket(const QByteArray &packetData);
    void handlePlaintextPacket(const QByteArray &packetData);
    void handleDisconnectPacket(const QByteArray &packetData);
};

#endif // PACKETHANDLER_H