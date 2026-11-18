#pragma once

#include <stdint.h>

namespace Revduino
{
    enum class Error : uint8_t
    {
        None,
        I2C,
        WrongDeviceId,
        ProximityOverflow,
        InvalidMultiplexerPort,
        InvalidMultiplexerAddress,
    };

    inline const char* error_string(Error error)
    {
        switch (error)
        {
        case Error::None:
            return "Success";
        case Error::I2C:
            return "I2C transfer failed";
        case Error::WrongDeviceId:
            return "Unexpected sensor part ID";
        case Error::ProximityOverflow:
            return "Proximity overflow";
        case Error::InvalidMultiplexerPort:
            return "Invalid multiplexer port (expected 0-7)";
        case Error::InvalidMultiplexerAddress:
            return "Invalid multiplexer address (expected 0x70-0x77)";
        }
        return "Unknown error";
    }
} // namespace Revduino
