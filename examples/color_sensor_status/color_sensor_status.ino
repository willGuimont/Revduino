#include <Arduino.h>
#include <Revduino.h>

Revduino::ColorSensorV3 sensor(Wire);
bool                    ready = false;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
}

void loop()
{
    if (!ready)
    {
        const auto error = sensor.begin();
        ready            = error == Revduino::Error::None;
        if (!ready)
        {
            Serial.println(Revduino::error_string(error));
            delay(1000);
        }
        return;
    }

    delay(10);
    Revduino::ColorSensorV3::MainStatus status;
    // Reading status clears flags, so handle both channels from this snapshot.
    const auto status_error = sensor.get_status(status);
    if (status_error != Revduino::Error::None)
    {
        Serial.println(Revduino::error_string(status_error));
        ready = false;
        return;
    }

    // Restore defaults after a power-up or supply disturbance before using measurements.
    if (status.power_on_status())
    {
        Serial.println(F("Sensor reset detected; reinitializing"));
        ready = false;
        return;
    }

    // Interrupts are disabled by default; these flags matter when interrupts are configured.
    if (status.ls_interrupt_status())
        Serial.println(F("Light sensor interrupt condition met"));
    if (status.ps_interrupt_status())
        Serial.println(F("Proximity interrupt condition met"));

    if (status.ls_data_status())
    {
        Revduino::RawColor color;
        const auto         color_error = sensor.get_raw_color(color);
        if (color_error == Revduino::Error::None)
        {
            Serial.print(F("RGB IR: "));
            Serial.print(color.r);
            Serial.print(' ');
            Serial.print(color.g);
            Serial.print(' ');
            Serial.print(color.b);
            Serial.print(' ');
            Serial.println(color.ir);
        }
        else
        {
            Serial.println(Revduino::error_string(color_error));
            ready = false;
            return;
        }
    }

    if (status.ps_data_status())
    {
        // Near/far is threshold-based, not a calibrated distance measurement.
        Serial.println(status.ps_logic_status() ? F("Object close") : F("Object far"));
        uint16_t   proximity;
        const auto proximity_error = sensor.get_proximity(proximity);
        if (proximity_error == Revduino::Error::None)
        {
            Serial.print(F("Proximity: "));
            Serial.println(proximity);
        }
        else
        {
            Serial.println(Revduino::error_string(proximity_error));
        }
    }
}
