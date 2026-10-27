#pragma once

#include <stdint.h>

namespace Revduino
{
    struct Color
    {
        float r = 0;
        float g = 0;
        float b = 0;

        constexpr Color() = default;
        constexpr Color(float r, float g, float b) : r(r), g(g), b(b) {}

        constexpr bool operator==(const Color& other) const
        {
            return r == other.r && g == other.g && b == other.b;
        }

        constexpr bool operator!=(const Color& other) const
        {
            return !(*this == other);
        }
    };

    struct RawColor
    {
        uint32_t r  = 0;
        uint32_t g  = 0;
        uint32_t b  = 0;
        uint32_t ir = 0;

        constexpr RawColor() = default;
        constexpr RawColor(uint32_t r, uint32_t g, uint32_t b, uint32_t ir) : r(r), g(g), b(b), ir(ir) {}
    };

    struct CIEColor
    {
        float x = 0;
        float y = 0;
        float z = 0;

        constexpr CIEColor() = default;
        constexpr CIEColor(float x, float y, float z) : x(x), y(y), z(z) {}

        constexpr double magnitude() const
        {
            return x + y + z;
        }

        constexpr double chromaticityX() const
        {
            return magnitude() == 0.0 ? 0.0 : x / magnitude();
        }

        constexpr double chromaticityY() const
        {
            return magnitude() == 0.0 ? 0.0 : y / magnitude();
        }
    };

} // namespace Revduino
