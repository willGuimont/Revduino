#include <Arduino.h>
#include <Revduino.h>

Revduino::ColorSensorV3 sensor(Wire);
bool                    ready = false;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    const auto error = sensor.begin();
    ready            = error == Revduino::Error::None;
    if (!ready)
        Serial.println(Revduino::error_string(error));
    else
        Serial.println(F("Color Sensor V3 initialized"));
}

void loop()
{
    if (!ready)
        return;

    Revduino::RawColor color;
    uint16_t           proximity;

    const auto color_error     = sensor.get_raw_color(color);
    const auto proximity_error = sensor.get_proximity(proximity);

    if (color_error == Revduino::Error::None)
    {
        Serial.print(F("R G B IR: "));
        Serial.print(color.r);
        Serial.print(' ');
        Serial.print(color.g);
        Serial.print(' ');
        Serial.print(color.b);
        Serial.print(' ');
        Serial.print(color.ir);
    }
    else
    {
        Serial.print(Revduino::error_string(color_error));
    }

    Serial.print(F(" | Proximity: "));
    if (proximity_error == Revduino::Error::None)
        Serial.println(proximity);
    else
        Serial.println(Revduino::error_string(proximity_error));

    delay(100);
}
