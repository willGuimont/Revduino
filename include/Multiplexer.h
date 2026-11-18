#pragma once

#include "Error.h"

#include <Adafruit_I2CDevice.h>

namespace Revduino
{
    class Multiplexer
    {
      public:
        // The address is 0x70 + (A2 << 2) + (A1 << 1) + A0, where each pin is 1 when tied high (to VIN) and 0
        // when tied to GND. The Adafruit breakout pulls A0-A2 low, so unconnected pins read 0; on other boards,
        // tie unused address pins to GND rather than leaving them floating.
        //   A2 A1 A0 | address
        //    0  0  0 | 0x70 (default)
        //    0  0  1 | 0x71
        //    0  1  0 | 0x72
        //    0  1  1 | 0x73
        //    1  0  0 | 0x74
        //    1  0  1 | 0x75
        //    1  1  0 | 0x76
        //    1  1  1 | 0x77
        static constexpr uint8_t DEFAULT_ADDRESS = 0x70;
        static constexpr uint8_t MAX_ADDRESS     = 0x77;

        // An address outside 0x70-0x77 makes begin() return Error::InvalidMultiplexerAddress.
        explicit Multiplexer(TwoWire& wire = Wire, uint8_t address = DEFAULT_ADDRESS);
        Multiplexer(Multiplexer&&)            = default;
        Multiplexer& operator=(Multiplexer&&) = default;

        Error begin();
        Error select(uint8_t port);
        Error stop();

      private:
        uint8_t            address;
        Adafruit_I2CDevice i2c_dev;
    };
} // namespace Revduino
