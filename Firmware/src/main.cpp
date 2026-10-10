#include <Arduino.h>

void setup() {
    pinMode(D8, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    digitalWrite(D8, !digitalRead(D8));
    Serial.println("Titan Driver Pad firmware running");
    delay(500);
}
