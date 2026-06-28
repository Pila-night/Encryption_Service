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

    // Принимает сырые байты от Network
    void handleData(const QByteArray &data);

signals:
    void publicKeyReceived(const QByteArray &key);
    void nonceReceived(const QByteArray &nonce);
    void signatureReceived(const QByteArray &signature);
    void ciphertextReceived(const QByteArray &payload);
    void plaintextReceived(const QByteArray &payload);
    void disconnectReceived();
    void errorOccurred(const QString &errorMessage);

private:
    void handlePublicKeyPacket(const std::vector<uint8_t> &data);
    void handleNoncePacket(const std::vector<uint8_t> &data);
    void handleSignaturePacket(const std::vector<uint8_t> &data);
    void handleCiphertextPacket(const std::vector<uint8_t> &data);
    void handlePlaintextPacket(const std::vector<uint8_t> &data);
    void handleDisconnectPacket(const std::vector<uint8_t> &data);
};

#endif // PACKETHANDLER_H