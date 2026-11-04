#include "Multiplexer.h"

namespace Revduino
{
    static constexpr int ADDRESS = 0x70;

    Multiplexer::Multiplexer(TwoWire& wire) : i2c_dev(ADDRESS, &wire) {}

    Error Multiplexer::begin()
    {
        if (!i2c_dev.begin())
            return Error::I2C;

        return Error::None;
    }

    Error Multiplexer::select(uint8_t port)
    {
        if (port > 7)
            return Error::InvalidPort;
        uint8_t data[] = {static_cast<uint8_t>(1 << port)};
        return i2c_dev.write(data, sizeof(data)) ? Error::None : Error::I2C;
    }

    Error Multiplexer::stop()
    {
        return select(0);
    }
} // namespace Revduino
