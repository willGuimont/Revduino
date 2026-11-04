#include "ColorSensorV3.h"

namespace Revduino
{
    static constexpr int                                ADDRESS             = 0x52;
    static constexpr int                                EXPECTED_PART_ID    = 0xC2;
    static constexpr ColorSensorV3::GainFactor          DEFAULT_GAIN        = ColorSensorV3::GainFactor::G3x;
    static constexpr ColorSensorV3::LEDPulseFrequency   DEFAULT_PULSE_FREQ  = ColorSensorV3::LEDPulseFrequency::Pf60kHz;
    static constexpr ColorSensorV3::LEDCurrent          DEFAULT_LED_CURRENT = ColorSensorV3::LEDCurrent::Pulse100mA;
    static constexpr uint8_t                            DEFAULT_PULSES      = 32;
    static constexpr ColorSensorV3::ProximityResolution DEFAULT_PROX_RES = ColorSensorV3::ProximityResolution::Pr11bit;
    static constexpr ColorSensorV3::ProximityMeasurementRate DEFAULT_PROX_RATE =
        ColorSensorV3::ProximityMeasurementRate::Mr50ms;
    static constexpr ColorSensorV3::ColorResolution      DEFAULT_COLOR_RES = ColorSensorV3::ColorResolution::Cr18bit;
    static constexpr ColorSensorV3::ColorMeasurementRate DEFAULT_COLOR_RATE =
        ColorSensorV3::ColorMeasurementRate::Mr100ms;

    // This is a transformation matrix given by the chip
    // manufacturer to transform the raw RGB to CIE XYZ
    constexpr double ColorSensorV3::Cmatrix[9] = {0.048112847,  0.289453437,  -0.084950826, -0.030754752, 0.339680186,
                                                  -0.071569905, -0.093947499, 0.072838494,  0.34024948};

    ColorSensorV3::ColorSensorV3(TwoWire& wire) : i2c_dev(ADDRESS, &wire) {}

    Error ColorSensorV3::begin()
    {
        if (!i2c_dev.begin())
            return Error::I2C;

        auto error = check_device_id();
        if (error != Error::None)
            return error;
        error = init_device();
        if (error != Error::None)
            return error;

        bool reset;
        return has_reset(reset);
    }

    Error ColorSensorV3::check_device_id()
    {
        uint8_t    id;
        const auto error = read8(Register::PartID, id);
        if (error != Error::None)
            return error;
        return id == EXPECTED_PART_ID ? Error::None : Error::WrongDeviceId;
    }

    Error ColorSensorV3::init_device()
    {
        const auto value = static_cast<uint8_t>(MainCtrlFields::RGBMode) |
                           static_cast<uint8_t>(MainCtrlFields::LightSensorEnable) |
                           static_cast<uint8_t>(MainCtrlFields::ProximitySensorEnable);

        auto error = write8(Register::MainCtrl, value);
        if (error != Error::None)
            return error;
        error = configure_proximity_sensor_led(DEFAULT_PULSE_FREQ, DEFAULT_LED_CURRENT, DEFAULT_PULSES);
        if (error != Error::None)
            return error;
        error = configure_proximity_sensor(DEFAULT_PROX_RES, DEFAULT_PROX_RATE);
        if (error != Error::None)
            return error;
        error = configure_color_sensor(DEFAULT_COLOR_RES, DEFAULT_COLOR_RATE);
        if (error != Error::None)
            return error;
        return set_gain(DEFAULT_GAIN);
    }

    Error ColorSensorV3::has_reset(bool& reset)
    {
        MainStatus status;
        const auto error = get_status(status);
        if (error == Error::None)
            reset = status.power_on_status();
        return error;
    }

    Error ColorSensorV3::get_status(ColorSensorV3::MainStatus& status)
    {
        return read(Register::MainStatus, &status.raw, 1);
    }

    Error ColorSensorV3::get_color(Color& color)
    {
        RawColor   raw_color = {};
        const auto error     = get_raw_color(raw_color);

        if (error == Error::None)
        {
            auto r   = static_cast<float>(raw_color.r);
            auto g   = static_cast<float>(raw_color.g);
            auto b   = static_cast<float>(raw_color.b);
            auto mag = r + g + b;

            if (mag == 0)
                color = {};
            else
                color = Color(r / mag, g / mag, b / mag);
        }

        return error;
    }

    Error ColorSensorV3::get_raw_color(RawColor& raw_color)
    {
        // Read 4 registers at once {ir, g, b, r}, see register order
        uint8_t raw[12];

        const auto error = read(Register::DataInfrared, raw, 12);

        if (error == Error::None)
        {
            const auto r  = to_20_bit(&raw[9]);
            const auto g  = to_20_bit(&raw[3]);
            const auto b  = to_20_bit(&raw[6]);
            const auto ir = to_20_bit(&raw[0]);

            raw_color = RawColor(r, g, b, ir);
        }

        return error;
    }

    Error ColorSensorV3::get_cie_color(CIEColor& cie_color)
    {
        RawColor   raw;
        const auto error = get_raw_color(raw);

        if (error == Error::None)
        {
            const auto x = Cmatrix[0] * raw.r + Cmatrix[1] * raw.g + Cmatrix[2] * raw.b;
            const auto y = Cmatrix[3] * raw.r + Cmatrix[4] * raw.g + Cmatrix[5] * raw.b;
            const auto z = Cmatrix[6] * raw.r + Cmatrix[7] * raw.g + Cmatrix[8] * raw.b;

            cie_color = CIEColor(x, y, z);
        }

        return error;
    }

    Error ColorSensorV3::get_ir(float& ir)
    {
        uint32_t   value = 0;
        const auto error = read20(Register::DataInfrared, value);

        if (error == Error::None)
        {
            ir = static_cast<float>(value);
        }

        return error;
    }

    Error ColorSensorV3::get_proximity(uint16_t& prox)
    {
        uint8_t    data[2];
        const auto error = read(Register::ProximityData, data, sizeof(data));
        if (error != Error::None)
            return error;

        if (data[1] & 0x08) // PS_DATA_1 overflow flag
            return Error::ProximityOverflow;

        prox = to_11_bit(data);
        return Error::None;
    }

    Error ColorSensorV3::set_gain(GainFactor gain)
    {
        return write8(Register::LightSensorGain, static_cast<uint8_t>(gain));
    }

    Error ColorSensorV3::configure_proximity_sensor_led(LEDPulseFrequency freq, LEDCurrent current, uint8_t pulses)
    {
        const auto pled = static_cast<uint8_t>(freq) | static_cast<uint8_t>(current);

        const auto error = write8(Register::ProximitySensorLED, pled);
        if (error != Error::None)
            return error;
        return write8(Register::ProximitySensorPulses, pulses);
    }

    Error ColorSensorV3::configure_proximity_sensor(ProximityResolution res, ProximityMeasurementRate rate)
    {
        const auto ps = static_cast<uint8_t>(res) | static_cast<uint8_t>(rate);
        return write8(Register::ProximitySensorRate, ps);
    }

    Error ColorSensorV3::configure_color_sensor(ColorResolution res, ColorMeasurementRate rate)
    {
        const auto cs = static_cast<uint8_t>(res) | static_cast<uint8_t>(rate);
        return write8(Register::LightSensorMeasurementRate, cs);
    }

    Error ColorSensorV3::is_connected()
    {
        return check_device_id();
    }

    Error ColorSensorV3::read(Register reg, uint8_t* data, size_t count)
    {
        const auto address = static_cast<uint8_t>(reg);
        return i2c_dev.write_then_read(&address, 1, data, count) ? Error::None : Error::I2C;
    }

    Error ColorSensorV3::write8(Register reg, uint8_t value)
    {
        const auto    address = static_cast<uint8_t>(reg);
        const uint8_t data[2] = {
            address,
            value,
        };
        return i2c_dev.write(data, sizeof(data)) ? Error::None : Error::I2C;
    }

    Error ColorSensorV3::read8(Register reg, uint8_t& value)
    {
        return read(reg, &value, 1);
    }

    Error ColorSensorV3::read11(Register reg, uint16_t& value)
    {
        uint8_t    data[2];
        const auto error = read(reg, data, sizeof(data));
        if (error != Error::None)
            return error;

        value = to_11_bit(data);

        return Error::None;
    }

    Error ColorSensorV3::read20(Register reg, uint32_t& value)
    {
        uint8_t    data[3];
        const auto error = read(reg, data, sizeof(data));
        if (error != Error::None)
            return error;

        value = to_20_bit(data);

        return Error::None;
    }

    uint16_t ColorSensorV3::to_11_bit(uint8_t* data)
    {
        return (static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8)) & 0x07FF;
    }

    uint32_t ColorSensorV3::to_20_bit(uint8_t* data)
    {
        // cast to u32, keeping 20 bits
        return (static_cast<uint32_t>(data[0]) | (static_cast<uint32_t>(data[1]) << 8) |
                (static_cast<uint32_t>(data[2]) << 16)) &
               0x0FFFFF;
    }
} // namespace Revduino
