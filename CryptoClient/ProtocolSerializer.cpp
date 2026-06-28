// ProtocolSerializer.cpp
#include "ProtocolSerializer.h"
#include <cstring>

namespace Protocol
{
namespace
{
// Фиксированный пакет (PublicKey, Nonce, Signature)
std::vector<uint8_t> serializeFixed(PacketType type, const std::vector<uint8_t>& payload, size_t payloadSize)
{
    if (payload.size() != payloadSize)
        throw std::runtime_error("Fixed packet: payload size mismatch");
    std::vector<uint8_t> buffer(1 + payloadSize);   // 1 байт — тип
    buffer[0] = static_cast<uint8_t>(type);
    std::memcpy(buffer.data() + 1, payload.data(), payloadSize);
    return buffer;
}
void deserializeFixed(const std::vector<uint8_t>& data, PacketType expectedType,
                      std::vector<uint8_t>& outPayload, size_t payloadSize)
{
    if (data.size() != 1 + payloadSize)
        throw std::runtime_error("Fixed packet: exact size mismatch");
    if (static_cast<PacketType>(data[0]) != expectedType)
        throw std::runtime_error("Fixed packet: unexpected type");
    outPayload.assign(data.begin() + 1, data.begin() + 1 + payloadSize);
}

// Пакет переменной длины (Ciphertext, Plaintext) — формат: [тип:1][длина:2][данные]
std::vector<uint8_t> serializeVariable(PacketType type, const std::vector<uint8_t>& payload)
{
    std::vector<uint8_t> buffer;
    buffer.reserve(1 + 2 + payload.size());   // тип + 2 байта длины + данные
    buffer.push_back(static_cast<uint8_t>(type));
    uint16_t len = static_cast<uint16_t>(payload.size());
    buffer.push_back(static_cast<uint8_t>(len >> 8));    // big-endian
    buffer.push_back(static_cast<uint8_t>(len & 0xFF));
    buffer.insert(buffer.end(), payload.begin(), payload.end());
    return buffer;
}
void deserializeVariable(const std::vector<uint8_t>& data, PacketType expectedType,
                         std::vector<uint8_t>& outPayload)
{
    size_t pos = 0;
    if (data.size() < 1 + 2)
        throw std::runtime_error("Variable packet: buffer too small");
    if (static_cast<PacketType>(data[pos]) != expectedType)
        throw std::runtime_error("Variable packet: unexpected type");
    pos += 1;
    uint16_t len = (static_cast<uint16_t>(data[pos]) << 8) | data[pos + 1];
    pos += 2;
    if (data.size() - pos != len)
        throw std::runtime_error("Variable packet: invalid payload size");
    outPayload.assign(data.begin() + pos, data.end());
}
}

// PublicKeyPacket
std::vector<uint8_t> Serializer::serialize(const PublicKeyPacket& packet)
{ return serializeFixed(packet.type, packet.key, PUBLIC_KEY_SIZE); }
void Serializer::deserialize(const std::vector<uint8_t>& data, PublicKeyPacket& packet)
{
    deserializeFixed(data, PacketType::PublicKey, packet.key, PUBLIC_KEY_SIZE);
    packet.type = PacketType::PublicKey;
}

// NoncePacket
std::vector<uint8_t> Serializer::serialize(const NoncePacket& packet)
{ return serializeFixed(packet.type, packet.nonce, NONCE_SIZE); }
void Serializer::deserialize(const std::vector<uint8_t>& data, NoncePacket& packet)
{
    deserializeFixed(data, PacketType::Nonce, packet.nonce, NONCE_SIZE);
    packet.type = PacketType::Nonce;
}

// SignaturePacket
std::vector<uint8_t> Serializer::serialize(const SignaturePacket& packet)
{ return serializeFixed(packet.type, packet.signature, SIGNATURE_SIZE); }
void Serializer::deserialize(const std::vector<uint8_t>& data, SignaturePacket& packet)
{
    deserializeFixed(data, PacketType::Signature, packet.signature, SIGNATURE_SIZE);
    packet.type = PacketType::Signature;
}

// CiphertextPacket
std::vector<uint8_t> Serializer::serialize(const CiphertextPacket& packet)
{ return serializeVariable(packet.type, packet.payload); }
void Serializer::deserialize(const std::vector<uint8_t>& data, CiphertextPacket& packet)
{
    deserializeVariable(data, PacketType::Ciphertext, packet.payload);
    packet.type = PacketType::Ciphertext;
}

// PlaintextPacket
std::vector<uint8_t> Serializer::serialize(const PlaintextPacket& packet)
{ return serializeVariable(packet.type, packet.payload); }
void Serializer::deserialize(const std::vector<uint8_t>& data, PlaintextPacket& packet)
{
    deserializeVariable(data, PacketType::Plaintext, packet.payload);
    packet.type = PacketType::Plaintext;
}

// DisconnectPacket
std::vector<uint8_t> Serializer::serialize(const DisconnectPacket& packet)
{ return { static_cast<uint8_t>(packet.type) }; }
void Serializer::deserialize(const std::vector<uint8_t>& data, DisconnectPacket& packet)
{
    if (data.size() != 1)
        throw std::runtime_error("DisconnectPacket: buffer must be exactly 1 byte");
    if (static_cast<PacketType>(data[0]) != PacketType::Disconnect)
        throw std::runtime_error("DisconnectPacket: unexpected type");
    packet.type = PacketType::Disconnect;
}

// peekType
PacketType Serializer::peekType(const std::vector<uint8_t>& data)
{
    if (data.empty())
        throw std::runtime_error("peekType: empty data");
    return static_cast<PacketType>(data[0]);
}

} // namespace Protocol