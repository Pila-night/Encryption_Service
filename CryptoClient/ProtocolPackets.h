// ProtocolPackets.h
#pragma once
#include <cstdint>
#include <vector>

namespace Protocol
{
inline constexpr size_t PUBLIC_KEY_SIZE  = 64;   // открытый ключ
inline constexpr size_t NONCE_SIZE       = 64;   // случайная последовательность
inline constexpr size_t SIGNATURE_SIZE   = 64;   // подпись

enum class PacketType : uint8_t
{
    PublicKey   = 0x01,
    Nonce       = 0x02,
    Signature   = 0x03,
    Ciphertext  = 0x04,   // зашифрованное сообщение
    Disconnect  = 0x05,
    Plaintext   = 0x06    // открытый текст
};

struct PublicKeyPacket
{
    PacketType type = PacketType::PublicKey;
    std::vector<uint8_t> key;
};
struct NoncePacket
{
    PacketType type = PacketType::Nonce;
    std::vector<uint8_t> nonce;
};
struct SignaturePacket
{
    PacketType type = PacketType::Signature;
    std::vector<uint8_t> signature;
};
struct CiphertextPacket
{
    PacketType type = PacketType::Ciphertext;
    std::vector<uint8_t> payload;   // зашифрованные данные
};
struct PlaintextPacket
{
    PacketType type = PacketType::Plaintext;
    std::vector<uint8_t> payload;   // открытый текст
};
struct DisconnectPacket
{
    PacketType type = PacketType::Disconnect;
};
} // namespace Protocol