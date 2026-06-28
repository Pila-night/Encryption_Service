#include "ProtocolAdapter.h"

namespace Protocol {

QByteArray Adapter::toQByteArray(const std::vector<uint8_t>& vec)
{
    return QByteArray(reinterpret_cast<const char*>(vec.data()),
                      static_cast<int>(vec.size()));
}

std::vector<uint8_t> Adapter::toVector(const QByteArray& arr)
{
    const uint8_t* begin = reinterpret_cast<const uint8_t*>(arr.constData());
    const uint8_t* end = begin + arr.size();
    return std::vector<uint8_t>(begin, end);
}

} // namespace Protocol