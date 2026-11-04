#pragma once

#include "Error.h"

#include <Adafruit_I2CDevice.h>

namespace Revduino
{
    class Multiplexer
    {
      public:
        explicit Multiplexer(TwoWire& wire = Wire);
        Multiplexer(Multiplexer&&)            = default;
        Multiplexer& operator=(Multiplexer&&) = default;

        Error begin();
        Error select(uint8_t port);
        Error stop();

      private:
        Adafruit_I2CDevice i2c_dev;
    };
} // namespace Revduino
