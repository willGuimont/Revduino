#include <Arduino.h>
#include <Revduino.h>

Revduino::ColorSensorV3 sensor;
bool ready = false;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    ready = sensor.begin();
    if (!ready) Serial.println(F("Color Sensor V3 initialization failed"));
}

void loop() {
    if (!ready) return;
    Revduino::RawColor color;
    uint16_t proximity;
    if (sensor.read_raw_color(color) && sensor.read_proximity(proximity)) {
        Serial.print(color.red);
        Serial.print(' ');
        Serial.print(color.green);
        Serial.print(' ');
        Serial.print(color.blue);
        Serial.print(' ');
        Serial.print(color.ir);
        Serial.print(' ');
        Serial.println(proximity);
    } else {
        Serial.println(F("I2C read failed"));
    }
    delay(100);
}
