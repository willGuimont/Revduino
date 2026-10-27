#pragma once

namespace Revduino
{
    struct RawColor
    {
        uint32_t red, green, blue, ir;
    };
    struct Color
    {
        float red, green, blue;
    };

    class ColorSensorV3
    {
      public:
        explicit ColorSensorV3(TwoWire& wire = Wire) : wire_(wire) {}
        // Call Wire.begin() first. No bus access occurs in the constructor.
        bool begin();
        bool is_connected();
        // On failure, output arguments are unchanged.
        bool read_raw_color(RawColor& color);
        bool read_color(Color& color);
        bool read_proximity(uint16_t& proximity);
        bool read_reset(bool& reset); // Reading clears the device's reset flag.

      private:
        bool     read(uint8_t reg, uint8_t* data, uint8_t size);
        bool     write(uint8_t reg, uint8_t value);
        TwoWire& wire_;
    };
} // namespace Revduino

