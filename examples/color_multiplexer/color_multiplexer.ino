#include <Arduino.h>
#include <Revduino.h>

Revduino::ColorSensorV3 sensor(Wire);
Revduino::Multiplexer   multiplexer(Wire);
bool                    ready = false;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    auto error = multiplexer.begin();
    if (error == Revduino::Error::None)
        error = multiplexer.select(1);
    if (error == Revduino::Error::None)
        error = sensor.begin();
    ready = error == Revduino::Error::None;
    if (!ready)
        Serial.println(Revduino::error_string(error));
}

void loop()
{
    if (!ready)
        return;

    Revduino::RawColor color;
    uint16_t           proximity;

    const auto color_error     = sensor.get_raw_color(color);
    const auto proximity_error = sensor.get_proximity(proximity);

    if (color_error == Revduino::Error::None && proximity_error == Revduino::Error::None)
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
        Serial.println(Revduino::error_string(color_error != Revduino::Error::None ? color_error : proximity_error));
    }

    delay(100);
}
