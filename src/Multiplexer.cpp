#include "Multiplexer.h"

namespace Revduino
{
    static constexpr int ADDRESS = 0x70;

    Multiplexer::Multiplexer(TwoWire& wire) : i2c_dev(ADDRESS, &wire) {}

    bool Multiplexer::begin()
    {
        if (!i2c_dev.begin())
            return false;

        return true;
    }

    bool Multiplexer::select(uint8_t port)
    {
        if (port > 7)
            return;
        uint8_t data[] = {1 << port};
        i2c_dev.write(data, sizeof(data));
    }
} // namespace Revduino
