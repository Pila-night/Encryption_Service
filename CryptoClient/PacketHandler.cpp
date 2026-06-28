#include "PacketHandler.h"
#include <QDebug>

PacketHandler::PacketHandler(QObject *parent)
    : QObject(parent)
{
}

PacketHandler::~PacketHandler()
{
}

void PacketHandler::reset()
{
    m_buffer.clear();
}

void PacketHandler::feedData(const QByteArray &data)
{
    if (data.isEmpty()) return;

    m_buffer.append(data);
    qDebug() << "[PacketHandler] Received" << data.size()
             << "bytes, buffer:" << m_buffer.size() << "bytes";

    while (tryProcessOnePacket()) {
    }
}

bool PacketHandler::tryProcessOnePacket()
{
    if (m_buffer.isEmpty()) return false;

    int expectedSize = expectedPacketSize(m_buffer);
    if (expectedSize < 0) return false;

    if (m_buffer.size() < expectedSize) {
        qDebug() << "[PacketHandler] Waiting for data: need" << expectedSize
                 << "bytes, have" << m_buffer.size();
        return false;
    }

    QByteArray packetData = m_buffer.left(expectedSize);
    m_buffer.remove(0, expectedSize);

    qDebug() << "[PacketHandler] Extracted packet, type: 0x"
             << Qt::hex << (int)(uint8_t)packetData[0]
             << Qt::dec << ", size:" << packetData.size();

    try {
        std::vector<uint8_t> rawData = Protocol::Adapter::toVector(packetData);
        Protocol::PacketType type = Protocol::Serializer::peekType(rawData);

        switch (type) {
        case Protocol::PacketType::PublicKey:
            handlePublicKeyPacket(packetData);
            break;
        case Protocol::PacketType::Nonce:
            handleNoncePacket(packetData);
            break;
        case Protocol::PacketType::Signature:
            handleSignaturePacket(packetData);
            break;
        case Protocol::PacketType::Ciphertext:
            handleCiphertextPacket(packetData);
            break;
        case Protocol::PacketType::Plaintext:
            handlePlaintextPacket(packetData);
            break;
        case Protocol::PacketType::Disconnect:
            handleDisconnectPacket(packetData);
            break;
        default:
            emit errorOccurred(QString("Unknown packet type: 0x%1")
                                   .arg(static_cast<int>(type), 2, 16, QChar('0')));
            break;
        }

        return true;

    } catch (const std::exception &e) {
        emit errorOccurred(QString("Packet processing error: %1").arg(e.what()));
        return false;
    }
}

int PacketHandler::expectedPacketSize(const QByteArray &data)
{
    if (data.isEmpty()) return -1;

    uint8_t type = static_cast<uint8_t>(data[0]);

    switch (type) {
    case static_cast<uint8_t>(Protocol::PacketType::PublicKey):
    case static_cast<uint8_t>(Protocol::PacketType::Nonce):
    case static_cast<uint8_t>(Protocol::PacketType::Signature):
        return 1 + 64;

    case static_cast<uint8_t>(Protocol::PacketType::Ciphertext):
    case static_cast<uint8_t>(Protocol::PacketType::Plaintext): {
        if (data.size() < 3) return -1;
        uint16_t len = (static_cast<uint16_t>(static_cast<uint8_t>(data[1])) << 8) |
                       static_cast<uint8_t>(data[2]);
        return 3 + len;
    }

    case static_cast<uint8_t>(Protocol::PacketType::Disconnect):
        return 1;

    default:
        return -1;
    }
}

void PacketHandler::handlePublicKeyPacket(const QByteArray &data)
{
    try {
        std::vector<uint8_t> rawData = Protocol::Adapter::toVector(data);
        Protocol::PublicKeyPacket packet;
        Protocol::Serializer::deserialize(rawData, packet);

        QByteArray key = Protocol::Adapter::toQByteArray(packet.key);
        qDebug() << "[PacketHandler] PublicKey received," << key.size() << "bytes";
        emit publicKeyReceived(key);
    } catch (const std::exception &e) {
        emit errorOccurred(QString("PublicKey error: %1").arg(e.what()));
    }
}

void PacketHandler::handleNoncePacket(const QByteArray &data)
{
    try {
        std::vector<uint8_t> rawData = Protocol::Adapter::toVector(data);
        Protocol::NoncePacket packet;
        Protocol::Serializer::deserialize(rawData, packet);

        QByteArray nonce = Protocol::Adapter::toQByteArray(packet.nonce);
        qDebug() << "[PacketHandler] Nonce received," << nonce.size() << "bytes";
        emit nonceReceived(nonce);
    } catch (const std::exception &e) {
        emit errorOccurred(QString("Nonce error: %1").arg(e.what()));
    }
}

void PacketHandler::handleSignaturePacket(const QByteArray &data)
{
    try {
        std::vector<uint8_t> rawData = Protocol::Adapter::toVector(data);
        Protocol::SignaturePacket packet;
        Protocol::Serializer::deserialize(rawData, packet);

        QByteArray signature = Protocol::Adapter::toQByteArray(packet.signature);
        qDebug() << "[PacketHandler] Signature received," << signature.size() << "bytes";
        emit signatureReceived(signature);
    } catch (const std::exception &e) {
        emit errorOccurred(QString("Signature error: %1").arg(e.what()));
    }
}

void PacketHandler::handleCiphertextPacket(const QByteArray &data)
{
    try {
        std::vector<uint8_t> rawData = Protocol::Adapter::toVector(data);
        Protocol::CiphertextPacket packet;
        Protocol::Serializer::deserialize(rawData, packet);

        QByteArray payload = Protocol::Adapter::toQByteArray(packet.payload);
        qDebug() << "[PacketHandler] Ciphertext received," << payload.size() << "bytes";
        emit ciphertextReceived(payload);
    } catch (const std::exception &e) {
        emit errorOccurred(QString("Ciphertext error: %1").arg(e.what()));
    }
}

void PacketHandler::handlePlaintextPacket(const QByteArray &data)
{
    try {
        std::vector<uint8_t> rawData = Protocol::Adapter::toVector(data);
        Protocol::PlaintextPacket packet;
        Protocol::Serializer::deserialize(rawData, packet);

        QByteArray payload = Protocol::Adapter::toQByteArray(packet.payload);
        qDebug() << "[PacketHandler] Plaintext received," << payload.size() << "bytes";
        emit plaintextReceived(payload);
    } catch (const std::exception &e) {
        emit errorOccurred(QString("Plaintext error: %1").arg(e.what()));
    }
}

void PacketHandler::handleDisconnectPacket(const QByteArray &data)
{
    try {
        std::vector<uint8_t> rawData = Protocol::Adapter::toVector(data);
        Protocol::DisconnectPacket packet;
        Protocol::Serializer::deserialize(rawData, packet);

        qDebug() << "[PacketHandler] Disconnect received";
        emit disconnectReceived();
    } catch (const std::exception &e) {
        emit errorOccurred(QString("Disconnect error: %1").arg(e.what()));
    }
}