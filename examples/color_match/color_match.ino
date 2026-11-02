#include <Arduino.h>
#include <Revduino.h>

// Calibrated target colors from REV Robotics Color Sensor V3 examples
constexpr Revduino::Color BLUE_TARGET(0.143f, 0.427f, 0.429f);
constexpr Revduino::Color GREEN_TARGET(0.197f, 0.561f, 0.240f);
constexpr Revduino::Color RED_TARGET(0.561f, 0.232f, 0.114f);
constexpr Revduino::Color YELLOW_TARGET(0.361f, 0.524f, 0.113f);

Revduino::ColorSensorV3 sensor;
Revduino::ColorMatch<4> color_matcher;
bool                    ready = false;

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    color_matcher.add_color(BLUE_TARGET);
    color_matcher.add_color(GREEN_TARGET);
    color_matcher.add_color(RED_TARGET);
    color_matcher.add_color(YELLOW_TARGET);

    ready = sensor.begin();
    if (!ready)
        Serial.println(F("Color Sensor V3 initialization failed"));
}

void loop()
{
    if (!ready)
        return;

    Revduino::Color detected_color;
    if (!sensor.get_color(detected_color))
    {
        Serial.println(F("Error reading Color Sensor V3"));
        delay(100);
        return;
    }

    Revduino::Color matched_color;
    float           confidence = 0.0f;

    if (color_matcher.match_color(detected_color, matched_color, confidence))
    {
        if (matched_color == BLUE_TARGET)
            Serial.print(F("Blue"));
        else if (matched_color == RED_TARGET)
            Serial.print(F("Red"));
        else if (matched_color == GREEN_TARGET)
            Serial.print(F("Green"));
        else if (matched_color == YELLOW_TARGET)
            Serial.print(F("Yellow"));
        else
            Serial.print(F("Unknown"));

        Serial.print(F(" (confidence: "));
        Serial.print(confidence, 3);
        Serial.println(')');
    }
    else
    {
        Serial.println(F("Unknown"));
    }

    delay(100);
}
