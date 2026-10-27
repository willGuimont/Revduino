# Revduino

Arduino library for non-CAN REV Robotics devices. For CAN devices, use [willGuimont/CanControl](https://github.com/willGuimont/CanControl).

The librairy currently supports:

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
    if (sensor.read_raw_color(color)) {
        // Use color.red, color.green, color.blue, color.ir.
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

## Building examples

Examples use CanControl's two-level selection: `REVDUINO_MAIN` enables the
example entry point, and one `EXAMPLE_*` flag selects the sketch. These flags
are for building this repository, not for projects consuming the library.

| Example | Selection flag | PlatformIO environments |
| --- | --- | --- |
| Raw RGB, infrared, and proximity | `EXAMPLE_COLOR_SENSOR` | `uno`, `mega`, `megaatmega1280` |
| Normalized RGB | `EXAMPLE_NORMALIZED_COLOR` | `uno_normalized`, `mega_normalized`, `megaatmega1280_normalized` |

```sh
pio run -e uno
pio run -e uno_normalized
# Add -t upload to upload the selected example.
```

Both sketches are also available directly in Arduino IDE under `examples/`;
no selection flags are needed there. Normalized RGB values sum to one (or zero
for black); they are not calibrated sRGB.

To add an example, put its sketch in `examples/<name>/<name>.ino`, add its
selection flag to the exactly-one check and include branch in `src/main.cpp`,
and add environments in `platformio.ini` using `${env.build_flags}` plus that
flag. Add those environments to `.github/workflows/ci.yml` so CI builds them.

## License and attribution

Revduino is independent and is not affiliated with, maintained by, or endorsed
by REV Robotics. The Color Sensor V3 driver is adapted from the bundled REVLib
2026.0.5 implementation under the BSD 3-Clause License; its copyright notice and
terms are retained in the driver and [LICENSE](LICENSE). Original reference code
and its licenses remain under `third_party/` and are not compiled into the library.
