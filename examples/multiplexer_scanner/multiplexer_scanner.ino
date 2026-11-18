#include <Arduino.h>
#include <Wire.h>

// Diagnoses multiplexer wiring: checks that the multiplexer answers, then
// looks for a Color Sensor V3 on each of its 8 ports.

static constexpr uint8_t MULTIPLEXER_ADDRESS = 0x70;
static constexpr uint8_t SENSOR_ADDRESS      = 0x52;

bool select_port(uint8_t port)
{
    Wire.beginTransmission(MULTIPLEXER_ADDRESS);
    Wire.write(static_cast<uint8_t>(1 << port));
    return Wire.endTransmission() == 0;
}

bool device_present(uint8_t address)
{
    Wire.beginTransmission(address);
    return Wire.endTransmission() == 0;
}

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    Serial.print(F("Multiplexer at 0x70: "));
    if (!device_present(MULTIPLEXER_ADDRESS))
    {
        Serial.println(F("NOT FOUND"));
        return;
    }
    Serial.println(F("OK"));

    bool found = false;
    for (uint8_t port = 0; port < 8; port++)
    {
        if (select_port(port) && device_present(SENSOR_ADDRESS))
        {
            Serial.print(F("Sensor found on port "));
            Serial.println(port);
            found = true;
        }
    }

    // Disable every channel
    Wire.beginTransmission(MULTIPLEXER_ADDRESS);
    Wire.write(static_cast<uint8_t>(0));
    Wire.endTransmission();

    if (!found)
        Serial.println(F("Sensor not found on any port"));
}

void loop() {}
