#include <Arduino.h>
#include <Revduino.h>

Revduino::ColorSensorV3 sensor;
bool                    ready = false;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    ready = sensor.begin();
    if (!ready)
        Serial.println(F("Color Sensor V3 initialization failed"));
}

void loop()
{
    if (!ready)
        return;

    Revduino::RawColor color;
    uint16_t           proximity;

    const auto success_color     = sensor.get_raw_color(color);
    const auto success_proximity = sensor.get_proximity(proximity);

    if (success_color && success_proximity)
    {
        Serial.print(color.r);
        Serial.print(' ');
        Serial.print(color.g);
        Serial.print(' ');
        Serial.print(color.b);
        Serial.print(' ');
        Serial.print(color.ir);
        Serial.print(' ');
        Serial.println(proximity);
    }
    else
    {
        Serial.println(F("Error reading Color Sensor V3"));
    }

    delay(100);
}
