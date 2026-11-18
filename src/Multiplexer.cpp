#include "Multiplexer.h"

namespace Revduino
{
    Multiplexer::Multiplexer(TwoWire& wire, uint8_t address) : address(address), i2c_dev(address, &wire) {}

    Error Multiplexer::begin()
    {
        if (address < DEFAULT_ADDRESS || address > MAX_ADDRESS)
            return Error::InvalidMultiplexerAddress;
        if (!i2c_dev.begin())
            return Error::I2C;

        return Error::None;
    }

    Error Multiplexer::select(uint8_t port)
    {
        if (port > 7)
            return Error::InvalidMultiplexerPort;
        uint8_t data[] = {static_cast<uint8_t>(1 << port)};
        return i2c_dev.write(data, sizeof(data)) ? Error::None : Error::I2C;
    }

    Error Multiplexer::stop()
    {
        uint8_t data[] = {0};
        return i2c_dev.write(data, sizeof(data)) ? Error::None : Error::I2C;
    }
} // namespace Revduino
