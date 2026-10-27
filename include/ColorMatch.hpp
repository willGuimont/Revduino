#pragma once

#include "Color.h"

#include <math.h>
#include <stddef.h>

namespace Revduino
{
    template <size_t MaxColor> class ColorMatch
    {
        static_assert(MaxColor > 0, "MaxColor must be greater than 0");

      public:
        ColorMatch() = default;

        bool add_color(const Color& color)
        {
            if (color_count >= MaxColor)
                return false;

            colors[color_count] = color;
            ++color_count;

            return true;
        }

        void set_confidence_threshold(float confidence)
        {
            if (confidence < 0.0f)
                confidence_level = 0.0f;
            else if (confidence > 1.0f)
                confidence_level = 1.0f;
            else
                confidence_level = confidence;
        }

        bool match_color(const Color& color, Color& out_color) const
        {
            float d;
            return match_color(color, out_color, d);
        }

        bool match_color(const Color& color, Color& out_color, float& out_confidence) const
        {
            const auto success = match_closest(color, out_color, out_confidence) && out_confidence > confidence_level;

            if (!success)
            {
                out_confidence = 0.0f;
            }

            return success;
        }

        bool match_closest(const Color& color, Color& out_color) const
        {
            float confidence;
            return match_closest(color, out_color, confidence);
        }

        bool match_closest(const Color& color, Color& out_color, float& out_confidence) const
        {
            const auto mag = color.r + color.g + color.b;
            if (mag > 0.0f && color_count > 0)
            {
                float  min_dist = calculate_distance(colors[0], color);
                size_t idx      = 0;

                for (size_t i = 1; i < color_count; ++i)
                {
                    auto target_distance = calculate_distance(colors[i], color);

                    if (target_distance < min_dist)
                    {
                        min_dist = target_distance;
                        idx      = i;
                    }
                }
                out_confidence = 1.0f - (min_dist / mag);
                out_color      = colors[idx];
                return true;
            }
            else
            {
                out_confidence = 0.0f;
                return false;
            }
        }

      private:
        static float calculate_distance(const Color& c1, const Color& c2)
        {
            const auto dr = c1.r - c2.r;
            const auto dg = c1.g - c2.g;
            const auto db = c1.b - c2.b;

            return sqrtf((dr * dr + dg * dg + db * db) / 2.0f);
        }

        Color  colors[MaxColor];
        size_t color_count      = 0;
        float  confidence_level = 0.95f;
    };
} // namespace Revduino
