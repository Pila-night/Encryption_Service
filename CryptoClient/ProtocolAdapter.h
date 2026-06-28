#ifndef PROTOCOLADAPTER_H
#define PROTOCOLADAPTER_H

#include <QByteArray>
#include <vector>
#include <cstdint>

namespace Protocol {

class Adapter {
public:
    // Конвертация std::vector<uint8_t> в QByteArray
    static QByteArray toQByteArray(const std::vector<uint8_t>& vec);

    // Конвертация QByteArray в std::vector<uint8_t>
    static std::vector<uint8_t> toVector(const QByteArray& arr);
};

} // namespace Protocol

#endif // PROTOCOLADAPTER_H