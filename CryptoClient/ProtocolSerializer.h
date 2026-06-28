// ProtocolSerializer.h
#pragma once
#include "ProtocolPackets.h"
#include <vector>
#include <cstdint>
#include <stdexcept>

namespace Protocol
{
class Serializer
{
public:
    static std::vector<uint8_t> serialize(const PublicKeyPacket&   packet);
    static std::vector<uint8_t> serialize(const NoncePacket&       packet);
    static std::vector<uint8_t> serialize(const SignaturePacket&   packet);
    static std::vector<uint8_t> serialize(const CiphertextPacket&  packet);
    static std::vector<uint8_t> serialize(const PlaintextPacket&   packet);
    static std::vector<uint8_t> serialize(const DisconnectPacket&  packet);

    static void deserialize(const std::vector<uint8_t>& data, PublicKeyPacket&   packet);
    static void deserialize(const std::vector<uint8_t>& data, NoncePacket&       packet);
    static void deserialize(const std::vector<uint8_t>& data, SignaturePacket&   packet);
    static void deserialize(const std::vector<uint8_t>& data, CiphertextPacket&  packet);
    static void deserialize(const std::vector<uint8_t>& data, PlaintextPacket&   packet);
    static void deserialize(const std::vector<uint8_t>& data, DisconnectPacket&  packet);

    static PacketType peekType(const std::vector<uint8_t>& data);
};
} // namespace Protocol