#include "ColorSensorV3.h"

namespace Revduino
{
    namespace
    {
        constexpr uint8_t address = 0x52;
        uint32_t          decode_color(const uint8_t* data)
        {
            return (uint32_t(data[0]) | (uint32_t(data[1]) << 8) | (uint32_t(data[2]) << 16)) & 0x03FFFF;
        }
    } // namespace

    bool ColorSensorV3::write(uint8_t reg, uint8_t value)
    {
        wire_.beginTransmission(address);
        wire_.write(reg);
        wire_.write(value);
        return wire_.endTransmission() == 0;
    }

    bool ColorSensorV3::read(uint8_t reg, uint8_t* data, uint8_t size)
    {
        wire_.beginTransmission(address);
        wire_.write(reg);
        if (wire_.endTransmission(false) != 0)
            return false;
        if (wire_.requestFrom(address, size) != size)
        {
            while (wire_.available())
                wire_.read();
            return false;
        }
        for (uint8_t i = 0; i < size; ++i)
        {
            int value = wire_.read();
            if (value < 0)
                return false;
            data[i] = static_cast<uint8_t>(value);
        }
        return true;
    }

    bool ColorSensorV3::is_connected()
    {
        uint8_t id;
        return read(0x06, &id, 1) && id == 0xC2;
    }

    bool ColorSensorV3::begin()
    {
        // REVLib defaults: RGB + proximity, 60kHz/100mA LED, 32 pulses,
        // 11-bit proximity every 50ms, 18-bit color every 100ms, 3x gain.
        bool reset;
        return is_connected() && write(0x00, 0x07) && write(0x01, 0x1E) && write(0x02, 32) && write(0x03, 0x1C) &&
               write(0x04, 0x22) && write(0x05, 1) && read_reset(reset);
    }

    bool ColorSensorV3::read_raw_color(RawColor& color)
    {
        uint8_t data[12];
        if (!read(0x0A, data, sizeof(data)))
            return false;
        color = {decode_color(data + 9), decode_color(data + 3), decode_color(data + 6), decode_color(data)};
        return true;
    }

    bool ColorSensorV3::read_color(Color& color)
    {
        RawColor raw;
        if (!read_raw_color(raw))
            return false;
        const float sum = raw.red + raw.green + raw.blue;
        color           = sum == 0 ? Color{0, 0, 0} : Color{raw.red / sum, raw.green / sum, raw.blue / sum};
        return true;
    }

    bool ColorSensorV3::read_proximity(uint16_t& proximity)
    {
        uint8_t data[2];
        if (!read(0x08, data, sizeof(data)))
            return false;
        proximity = (uint16_t(data[0]) | (uint16_t(data[1]) << 8)) & 0x7FF;
        return true;
    }

    bool ColorSensorV3::read_reset(bool& reset)
    {
        uint8_t status;
        if (!read(0x07, &status, 1))
            return false;
        reset = (status & 0x20) != 0;
        return true;
    }
} // namespace Revduino
