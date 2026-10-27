#include <Arduino.h>
#include <Revduino.h>

Revduino::ColorSensorV3 sensor;
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
        ready = sensor.begin();
        if (!ready)
        {
            Serial.println(F("Color Sensor V3 initialization failed; retrying"));
            delay(1000);
        }
        return;
    }

    delay(10);
    Revduino::ColorSensorV3::MainStatus status;
    // Reading status clears flags, so handle both channels from this snapshot.
    if (!sensor.get_status(status))
    {
        Serial.println(F("Status read failed; reinitializing"));
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
        if (sensor.get_raw_color(color))
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
            Serial.println(F("Color read failed; reinitializing"));
            ready = false;
            return;
        }
    }

    if (status.ps_data_status())
    {
        // Near/far is threshold-based, not a calibrated distance measurement.
        Serial.println(status.ps_logic_status() ? F("Object close") : F("Object far"));
        uint16_t proximity;
        if (sensor.get_proximity(proximity))
        {
            Serial.print(F("Proximity: "));
            Serial.println(proximity);
        }
        else
        {
            Serial.println(F("Proximity read failed or overflowed"));
        }
    }
}
