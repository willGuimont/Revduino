# Revduino

[![CI](https://github.com/willGuimont/Revduino/actions/workflows/ci.yml/badge.svg)](https://github.com/willGuimont/Revduino/actions/workflows/ci.yml)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/willguimont/library/Revduino.svg)](https://registry.platformio.org/libraries/willguimont/Revduino)

Arduino library for non-CAN REV Robotics devices. For CAN devices, use [willGuimont/CanControl](https://github.com/willGuimont/CanControl).

Currently supports:

- [Color Sensor V3](https://docs.revrobotics.com/rev-crossover-products/sensors/color-sensor)

## Install and use

For PlatformIO, add this repository to your project's environment:

```ini
lib_deps = 
    https://github.com/willGuimont/Revduino.git
```

For Arduino IDE, install the repository ZIP with **Sketch > Include Library >
Add .ZIP Library**. Open `examples/color_sensor/color_sensor.ino`.

```cpp
#include <Revduino.h>

Revduino::ColorSensorV3 sensor;

void setup() {
    Wire.begin();
    if (!sensor.begin()) {
        // Device missing, wrong part ID, or I²C configuration failed.
    }
}

void loop() {
    Revduino::RawColor color;
    if (sensor.get_raw_color(color)) {
        // Use color.r, color.g, color.b, color.ir.
    }
    delay(100);
}
```

The constructor performs no I/O. Initialize the bus before `begin()`. You can
pass another `TwoWire` instance to the constructor. The sensor address is fixed
at `0x52`; multiple sensors on one bus require hardware address isolation such
as an I²C multiplexer.

Connect through the board's SDA/SCL pins and a common ground. Check the sensor's
power and I/O voltage requirements before connecting it to a 5 V Arduino; use
appropriate level shifting. Board compilation does not validate wiring.

## Build and test

The [color sensor example](examples/color_sensor/color_sensor.ino) prints raw
RGB, infrared, and proximity readings. Build it for Arduino Uno or Mega:

```sh
pio run -e uno -e mega
# Add -t upload with one environment to upload to a board.
pio test -e native
```

GitHub Actions builds both boards and runs native tests on every push and pull
request. Native tests run on your computer without a board; the initial suite
checks black-color chromaticity, not I²C communication.

Add host tests under `test/test_<name>/test_main.cpp` using Unity and the existing
suite as a template. The native environment excludes Arduino source files.

To add an example, add its sketch under `examples/`, a selection flag in
`src/main.cpp`, and an environment extending `avr` in `platformio.ini`. Add that
environment to the CI build matrix in `.github/workflows/ci.yml`.

## License and attribution

Revduino is independent and is not affiliated with, maintained by, or endorsed
by REV Robotics. The Color Sensor V3 driver is adapted from the bundled REVLib
2026.0.5 implementation under the BSD 3-Clause License; its copyright notice and
terms are retained in the driver and [LICENSE](LICENSE). Original reference code
and its licenses remain under `third_party/` and are not compiled into the library.
