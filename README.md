# Revduino

[![CI](https://github.com/willGuimont/Revduino/actions/workflows/ci.yml/badge.svg)](https://github.com/willGuimont/Revduino/actions/workflows/ci.yml)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/willguimont/library/Revduino.svg)](https://registry.platformio.org/libraries/willguimont/Revduino)

Arduino library for non-CAN REV Robotics devices. For CAN devices, use [willGuimont/CanControl](https://github.com/willGuimont/CanControl).

Currently supports:

- [Color Sensor V3](https://docs.revrobotics.com/rev-crossover-products/sensors/color-sensor)
  - Based on the [APDS-9151 by Broadcom](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf)

## Install and use

For PlatformIO, add this repository to your project's environment:

```ini
lib_deps = 
    https://github.com/willGuimont/Revduino.git
```

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

### Build commands

The [color sensor example](examples/color_sensor/color_sensor.ino) prints raw
RGB, infrared, and proximity readings. Build it for Arduino Uno or Mega:

```sh
pio run -e uno -e mega
# Build the status example for both boards:
pio run -e uno_status -e mega_status
# Build the color match example for both boards:
pio run -e uno_match -e mega_match
# Add -t upload with one environment to upload to a board.
pio test -e native
```

GitHub Actions builds all examples for both boards and runs native tests on every push and pull
request. Native tests run on your computer without a board; the initial suite
checks black-color chromaticity, not I²C communication.

Add host tests under `test/test_<name>/test_main.cpp` using Unity and the existing
suite as a template. The native environment excludes Arduino source files.

To add an example, add its sketch under `examples/`, a selection flag in
`src/main.cpp`, and an environment extending `avr` in `platformio.ini`. Add that
environment to the CI build matrix in `.github/workflows/ci.yml`.

### Publishing to PlatformIO Registry

1. Bump `"version"` in [`library.json`](library.json).
2. Run tests:
   ```bash
   pio test -e native
   ```
3. Log in (first time only):
   ```bash
   pio account login
   ```
4. Publish:
   ```bash
   pio pkg publish
   ```
   *(Or with token in CI: `export PLATFORMIO_AUTH_TOKEN="<token>" && pio pkg publish --no-interactive`)*

## Roadmap

### ColorSensorV3

Basic polling, measurement configuration, and status reporting are supported.
The following features are not yet implemented:

| Feature | What to do | How to implement | Spec pages |
| --- | --- | --- | --- |
| Interrupt configuration | Enable proximity/light interrupts and choose the light channel. | Add `INT_CFG` (`0x19`): proximity enable bit 0, light enable bit 2, and channel bits 5–4 (`0` IR, `1` green/ALS, `2` red, `3` blue). Preserve unrelated bits with read–modify–write. | [25](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=25) |
| Interrupt persistence | Require consecutive qualifying measurements to reduce noisy events. | Add `INT_PST` (`0x1A`): proximity in bits 3–0, light in bits 7–4. Validate 1–16 samples and encode `samples - 1`. | [25](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=25) |
| Interrupt thresholds | Configure the measurement range that triggers an interrupt when exceeded. | Write proximity upper/lower thresholds at `0x1B–0x1C` / `0x1D–0x1E` (11-bit), and light upper/lower thresholds at `0x21–0x23` / `0x24–0x26` (20-bit), least-significant byte first. Validate widths and `lower <= upper`; configure with the interrupt source disabled, then enable it last. | [26](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=26), [27](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=27), [28](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=28) |
| Light variation interrupts | Trigger on changes between light measurements instead of fixed thresholds. | Set `INT_CFG` (`0x19`) bit 3 and configure `LS_THRES_VAR` (`0x27`) bits 2–0: code `n` selects `8 << n` counts (`n = 0…7`). Also select a channel, enable light interrupts, and configure persistence. | [11](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=11), [25](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=25), [28](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=28) |
| Proximity logic output | Signal threshold-based near/far state through INT. | Set `INT_CFG` (`0x19`) bit 1 and configure proximity thresholds. This mode maintains the output state between measurements and takes priority over light interrupt signaling on INT. | [11](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=11), [12](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=12), [25](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=25) |
| Proximity offset cancellation | Compensate for background reflections from the installed enclosure. | Add `PS_CAN` (`0x1F–0x20`): digital offset bits 7–0 at `0x1F`, bits 10–8 at `0x20[2:0]`, and analog cancellation at `0x20[7:3]`. Validate field widths, preserve the other field, and document calibration using measurements from the actual enclosure. | [11](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=11), [27](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=27) |
| Sensor enable and mode controls | Turn light/proximity sensing on independently and select ALS or RGB operation. | Update `MAIN_CTRL` (`0x00`): proximity enable bit 0, light enable bit 1, RGB mode bit 2 (`0` ALS/IR, `1` RGB/IR). Preserve other settings with read–modify–write. | [17](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=17) |
| Software reset | Reset the sensor and restore configuration. | Set `MAIN_CTRL` (`0x00`) bit 4. The reset command intentionally receives no ACK; verify recovery with bounded retries before reinitializing and reapplying custom settings. | [10](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=10), [17](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=17) |
| Sleep after interrupt | Stop the corresponding sensor after an interrupt. | Set `MAIN_CTRL` (`0x00`) bit 5 for light or bit 6 for proximity. Handle the status event, then explicitly re-enable sensing when another measurement is needed. | [17](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=17) |
| Detailed errors | Distinguish proximity overflow from communication failure. | Return separate error values for failed I²C reads and `PS_DATA_1` (`0x09`) bit 3. Keep output values unchanged on failure. | [22](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=22) |
| Interrupt-driven example | Service events without continuous status polling, if INT is accessible on the board. | Connect the active-low, open-drain INT signal with an appropriate pull-up. Let the MCU interrupt handler set a flag; perform I²C and logging in `loop()`. Read `MAIN_STATUS` (`0x07`) once per event and handle all flags; also service an already-low INT line. | [12](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=12), [21](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=21), [25](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=25), [29](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf#page=29) |

Register details: [APDS-9151 datasheet](https://download.mikroe.com/documents/datasheets/APDS-9151%20Datasheet.pdf),
pages 17 and 21–29. Check every I²C operation for failure; a failed multi-register
configuration may leave only some settings applied.

## License and attribution

Revduino is independent and is not affiliated with, maintained by, or endorsed
by REV Robotics. Revduino uses the [GNU GPL v3](LICENSE). The Color Sensor V3
driver is adapted from REVLib 2026.0.5, which carries a BSD 3-Clause notice.
The applicable copyright notices and license terms are retained in
[THIRD_PARTY_NOTICES.txt](THIRD_PARTY_NOTICES.txt), included in the PlatformIO package.
Original reference code and its notices are in `third_party/`, which is neither
compiled into the library nor included in the PlatformIO package.
