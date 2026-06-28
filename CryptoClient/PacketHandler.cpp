#include "PacketHandler.h"
#include <QDebug>

PacketHandler::PacketHandler(QObject *parent)
    : QObject(parent)
{
}

PacketHandler::~PacketHandler()
{
}

void PacketHandler::handleData(const QByteArray &data)
{
    if (data.isEmpty()) {
        return;
    }

    try {
        // Конвертируем QByteArray → vector
        std::vector<uint8_t> rawData = Protocol::Adapter::toVector(data);

        // Определяем тип пакета
        Protocol::PacketType type = Protocol::Serializer::peekType(rawData);

        // Обрабатываем в зависимости от типа
        switch (type) {
        case Protocol::PacketType::PublicKey:
            handlePublicKeyPacket(rawData);
            break;
        case Protocol::PacketType::Nonce:
            handleNoncePacket(rawData);
            break;
        case Protocol::PacketType::Signature:
            handleSignaturePacket(rawData);
            break;
        case Protocol::PacketType::Ciphertext:
            handleCiphertextPacket(rawData);
            break;
        case Protocol::PacketType::Plaintext:
            handlePlaintextPacket(rawData);
            break;
        case Protocol::PacketType::Disconnect:
            handleDisconnectPacket(rawData);
            break;
        default:
            emit errorOccurred(QString("Неизвестный тип пакета: 0x%1")
                                   .arg(static_cast<int>(type), 2, 16, QChar('0')));
            break;
        }
    }
    catch (const std::exception &e) {
        emit errorOccurred(QString("Ошибка обработки пакета: %1").arg(e.what()));
    }
    catch (...) {
        emit errorOccurred("Неизвестная ошибка при обработке пакета");
    }
}

void PacketHandler::handlePublicKeyPacket(const std::vector<uint8_t> &data)
{
    try {
        Protocol::PublicKeyPacket packet;
        Protocol::Serializer::deserialize(data, packet);

        QByteArray key = Protocol::Adapter::toQByteArray(packet.key);

        qDebug() << "[PacketHandler] Получен PublicKey, размер:" << key.size() << "байт";
        emit publicKeyReceived(key);
    }
    catch (const std::exception &e) {
        emit errorOccurred(QString("Ошибка десериализации PublicKey: %1").arg(e.what()));
    }
}

void PacketHandler::handleNoncePacket(const std::vector<uint8_t> &data)
{
    try {
        Protocol::NoncePacket packet;
        Protocol::Serializer::deserialize(data, packet);

        QByteArray nonce = Protocol::Adapter::toQByteArray(packet.nonce);

        qDebug() << "[PacketHandler] Получен Nonce, размер:" << nonce.size() << "байт";
        emit nonceReceived(nonce);
    }
    catch (const std::exception &e) {
        emit errorOccurred(QString("Ошибка десериализации Nonce: %1").arg(e.what()));
    }
}

void PacketHandler::handleSignaturePacket(const std::vector<uint8_t> &data)
{
    try {
        Protocol::SignaturePacket packet;
        Protocol::Serializer::deserialize(data, packet);

        QByteArray signature = Protocol::Adapter::toQByteArray(packet.signature);

        qDebug() << "[PacketHandler] Получена Signature, размер:" << signature.size() << "байт";
        emit signatureReceived(signature);
    }
    catch (const std::exception &e) {
        emit errorOccurred(QString("Ошибка десериализации Signature: %1").arg(e.what()));
    }
}

void PacketHandler::handleCiphertextPacket(const std::vector<uint8_t> &data)
{
    try {
        Protocol::CiphertextPacket packet;
        Protocol::Serializer::deserialize(data, packet);

        QByteArray payload = Protocol::Adapter::toQByteArray(packet.payload);

        qDebug() << "[PacketHandler] Получен Ciphertext, размер:" << payload.size() << "байт";
        emit ciphertextReceived(payload);
    }
    catch (const std::exception &e) {
        emit errorOccurred(QString("Ошибка десериализации Ciphertext: %1").arg(e.what()));
    }
}

void PacketHandler::handlePlaintextPacket(const std::vector<uint8_t> &data)
{
    try {
        Protocol::PlaintextPacket packet;
        Protocol::Serializer::deserialize(data, packet);

        QByteArray payload = Protocol::Adapter::toQByteArray(packet.payload);

        qDebug() << "[PacketHandler] Получен Plaintext, размер:" << payload.size() << "байт";
        emit plaintextReceived(payload);
    }
    catch (const std::exception &e) {
        emit errorOccurred(QString("Ошибка десериализации Plaintext: %1").arg(e.what()));
    }
}

void PacketHandler::handleDisconnectPacket(const std::vector<uint8_t> &data)
{
    try {
        Protocol::DisconnectPacket packet;
        Protocol::Serializer::deserialize(data, packet);

        qDebug() << "[PacketHandler] Получен Disconnect";
        emit disconnectReceived();
    }
    catch (const std::exception &e) {
        emit errorOccurred(QString("Ошибка десериализации Disconnect: %1").arg(e.what()));
    }
}