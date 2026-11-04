#pragma once

#include <Adafruit_I2CDevice.h>

namespace Revduino
{
    class Multiplexer
    {
      public:
        explicit Multiplexer(TwoWire& wire = Wire);
        Multiplexer(Multiplexer&&)            = default;
        Multiplexer& operator=(Multiplexer&&) = default;

        bool begin();
        bool select(uint8_t port);

      private:
        Adafruit_I2CDevice i2c_dev;
    };
} // namespace Revduino
