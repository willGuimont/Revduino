#pragma once

#include "Color.h"

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <stdint.h>

namespace Revduino
{
    class ColorSensorV3
    {
      public:
        enum class GainFactor
        {
            G1x  = 0,
            G3x  = 1,
            G6x  = 2,
            G9x  = 3,
            G18x = 4
        };

        enum class LEDPulseFrequency
        {
            Pf60kHz  = 0x18,
            Pf70kHz  = 0x40,
            Pf80kHz  = 0x28,
            Pf90kHz  = 0x30,
            Pf100kHz = 0x38,
        };

        enum class LEDCurrent
        {
            Pulse2mA   = 0,
            Pulse5mA   = 1,
            Pulse10mA  = 2,
            Pulse25mA  = 3,
            Pulse50mA  = 4,
            Pulse75mA  = 5,
            Pulse100mA = 6,
            Pulse125mA = 7,
        };

        enum class ProximityResolution
        {
            Pr8bit  = 0x00,
            Pr9bit  = 0x08,
            Pr10bit = 0x10,
            Pr11bit = 0x18,
        };

        enum class ProximityMeasurementRate
        {
            Mr6ms   = 1,
            Mr12ms  = 2,
            Mr25ms  = 3,
            Mr50ms  = 4,
            Mr100ms = 5,
            Mr200ms = 6,
            Mr400ms = 7,
        };

        enum class ColorResolution
        {
            Cr20bit = 0x00,
            Cr19bit = 0x10,
            Cr18bit = 0x20,
            Cr17bit = 0x30,
            Cr16bit = 0x40,
            Cr13bit = 0x50,
        };

        enum class ColorMeasurementRate
        {
            Mr25ms   = 0,
            Mr50ms   = 1,
            Mr100ms  = 2,
            Mr200ms  = 3,
            Mr500ms  = 4,
            Mr1000ms = 5,
            Mr2000ms = 7,
        };

        explicit ColorSensorV3(TwoWire& wire = Wire);
        ColorSensorV3(ColorSensorV3&&)            = default;
        ColorSensorV3& operator=(ColorSensorV3&&) = default;

        bool begin();

        bool get_color(Color& color);
        bool get_raw_color(RawColor& raw_color);
        bool get_cie_color(CIEColor& cie_color);
        bool get_ir(float& ir);
        bool get_proximity(uint16_t& prox);

        bool set_gain(GainFactor gain);
        bool configure_proximity_sensor_led(LEDPulseFrequency freq, LEDCurrent current, uint8_t pulses);
        bool configure_proximity_sensor(ProximityResolution res, ProximityMeasurementRate rate);
        bool configure_color_sensor(ColorResolution res, ColorMeasurementRate rate);

        bool has_reset();
        bool is_connected();

      private:
        enum class Register
        {
            MainCtrl                   = 0x00,
            ProximitySensorLED         = 0x01,
            ProximitySensorPulses      = 0x02,
            ProximitySensorRate        = 0x03,
            LightSensorMeasurementRate = 0x04,
            LightSensorGain            = 0x05,
            PartID                     = 0x06,
            MainStatus                 = 0x07,
            ProximityData              = 0x08,
            DataInfrared               = 0x0A,
            DataGreen                  = 0x0D,
            DataBlue                   = 0x10,
            DataRed                    = 0x13
        };

        enum class MainCtrlFields
        {
            ProximitySensorEnable = 0x01,
            LightSensorEnable     = 0x02,
            RGBMode               = 0x04
        };

        struct MainStatus
        {
            uint8_t raw = 0;

            constexpr bool ps_data_status() const
            {
                return raw & (1u << 0);
            }

            constexpr bool ps_interrupt_status() const
            {
                return raw & (1u << 1);
            }

            constexpr bool ps_logic_status() const
            {
                return raw & (1u << 2);
            }

            constexpr bool ls_data_status() const
            {
                return raw & (1u << 3);
            }

            constexpr bool ls_interrupt_status() const
            {
                return raw & (1u << 4);
            }

            constexpr bool power_on_status() const
            {
                return raw & (1u << 5);
            }
        };

        bool get_status(MainStatus& status);
        bool check_device_id();
        bool init_device();
        bool read(Register reg, uint8_t* data, size_t count);
        bool write8(Register reg, uint8_t value);
        bool read8(Register reg, uint8_t& value);
        bool read11(Register reg, uint16_t& value);
        bool read20(Register reg, uint32_t& value);

        uint16_t to_11_bit(uint8_t* data);
        uint32_t to_20_bit(uint8_t* data);

      private:
        static const double Cmatrix[9];
        Adafruit_I2CDevice  i2c_dev;
    };
} // namespace Revduino
