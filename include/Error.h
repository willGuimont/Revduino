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
        InvalidPort,
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
        case Error::InvalidPort:
            return "Invalid multiplexer port (expected 0-7)";
        }
        return "Unknown error";
    }
} // namespace Revduino
