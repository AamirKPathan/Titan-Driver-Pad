#include <Arduino.h>

constexpr uint32_t SERIAL_BAUD_RATE = 115200;
constexpr uint32_t STARTUP_DELAY_MS = 200;
constexpr uint32_t STATUS_INTERVAL_MS = 1000;

uint32_t lastStatusTime = 0;
uint32_t loopCount = 0;

bool firmwareReady = false;

void initializeFirmware();
void printStartupMessage();
void updateStatus();
void processInputs();
void updateDisplay();
void sendControllerState();

void setup() {
    initializeFirmware();
}

void loop() {
    processInputs();
    updateDisplay();
    sendControllerState();
    updateStatus();

    loopCount++;
    delay(1);
}

void initializeFirmware() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(STARTUP_DELAY_MS);
    printStartupMessage();
    firmwareReady = true;
    lastStatusTime = millis();
}

void printStartupMessage() {
    Serial.println();
    Serial.println("========================================");
    Serial.println("       FRC 5719 TITAN DRIVER PAD");
    Serial.println("========================================");
    Serial.println("Hardware: Seeed Studio XIAO ESP32-C3");
    Serial.println("Framework: Arduino");
    Serial.println("Transport: USB Serial");
    Serial.println("----------------------------------------");
    Serial.println("Firmware initialized.");
    Serial.println("Switch matrix: Not configured yet");
    Serial.println("OLED display: Not initialized yet");
    Serial.println("Gamepad bridge: Not connected yet");
    Serial.println("========================================");
    Serial.println();
}

void processInputs() {
}

void updateDisplay() {
}

void sendControllerState() {
}

void updateStatus() {
    const uint32_t currentTime = millis();

    if (currentTime - lastStatusTime >= STATUS_INTERVAL_MS) {
        lastStatusTime = currentTime;

        if (firmwareReady) {
            Serial.println("Titan Driver Pad firmware running");
        }
    }
}