
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================================================
// FRC 5719 - TITAN DRIVER PAD
// Board: Seeed Studio XIAO ESP32-C3
// ============================================================

// Serial
constexpr uint32_t SERIAL_BAUD_RATE = 115200;
constexpr uint32_t SERIAL_INTERVAL_MS = 20;

// OLED
constexpr uint8_t OLED_SDA_PIN = 6;
constexpr uint8_t OLED_SCL_PIN = 7;
constexpr uint8_t OLED_ADDRESS = 0x3C;

constexpr int OLED_WIDTH = 128;
constexpr int OLED_HEIGHT = 32;
constexpr int OLED_RESET_PIN = -1;

Adafruit_SSD1306 display(
    OLED_WIDTH,
    OLED_HEIGHT,
    &Wire,
    OLED_RESET_PIN
);

bool oledReady = false;

// Switch matrix
constexpr uint8_t ROW_COUNT = 4;
constexpr uint8_t COL_COUNT = 2;
constexpr uint8_t SWITCH_COUNT = 8;

constexpr uint8_t ROW_PINS[ROW_COUNT] = {2, 3, 4, 5};
constexpr uint8_t COL_PINS[COL_COUNT] = {8, 9};

// Rows: 1&5, 2&6, 3&7, 4&8
// Columns: 1-4 and 5-8
constexpr uint8_t MATRIX_MAP[ROW_COUNT][COL_COUNT] = {
    {0, 4},
    {1, 5},
    {2, 6},
    {3, 7}
};

// Debouncing
constexpr uint32_t DEBOUNCE_MS = 20;

bool rawSwitches[SWITCH_COUNT] = {};
bool candidateSwitches[SWITCH_COUNT] = {};
bool switchStates[SWITCH_COUNT] = {};

uint32_t candidateChangedAt[SWITCH_COUNT] = {};

// Controller states
bool stickUp = false;
bool stickLeft = false;
bool stickDown = false;
bool stickRight = false;

bool buttonL1 = false;
bool buttonL2 = false;
bool buttonR1 = false;
bool buttonR2 = false;

// Timing
constexpr uint32_t OLED_INTERVAL_MS = 100;

uint32_t lastSerialTime = 0;
uint32_t lastOLEDTime = 0;

// Function declarations
void initializeMatrix();
void initializeOLED();
void scanMatrix();
void updateDebounce();
void updateControllerState();
void sendControllerState();
void updateOLED();

// ============================================================
// SETUP
// ============================================================

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(300);

    Serial.println();
    Serial.println("FRC 5719 TITAN DRIVER PAD");
    Serial.println("Starting firmware...");

    initializeMatrix();
    initializeOLED();

    Serial.println("Switch matrix initialized.");

    if (oledReady) {
        Serial.println("OLED initialized.");
    } else {
        Serial.println("WARNING: OLED not detected.");
        Serial.println("Check SDA, SCL, power and ground.");
    }

    Serial.println(
        "Format: STATE,UP,LEFT,DOWN,RIGHT,L1,L2,R1,R2"
    );
    Serial.println("Firmware ready.");
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {
    scanMatrix();
    updateDebounce();
    updateControllerState();

    const uint32_t now = millis();

    if (now - lastSerialTime >= SERIAL_INTERVAL_MS) {
        lastSerialTime = now;
        sendControllerState();
    }

    if (now - lastOLEDTime >= OLED_INTERVAL_MS) {
        lastOLEDTime = now;
        updateOLED();
    }

    delay(1);
}

// ============================================================
// MATRIX INITIALIZATION
// ============================================================

void initializeMatrix() {
    for (uint8_t col = 0; col < COL_COUNT; col++) {
        pinMode(COL_PINS[col], INPUT_PULLUP);
    }

    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        pinMode(ROW_PINS[row], OUTPUT);
        digitalWrite(ROW_PINS[row], HIGH);
    }

    const uint32_t now = millis();

    for (uint8_t i = 0; i < SWITCH_COUNT; i++) {
        rawSwitches[i] = false;
        candidateSwitches[i] = false;
        switchStates[i] = false;
        candidateChangedAt[i] = now;
    }
}

// ============================================================
// MATRIX SCANNING
// ============================================================

void scanMatrix() {
    // All rows inactive.
    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        digitalWrite(ROW_PINS[row], HIGH);
    }

    // Scan one row at a time.
    for (uint8_t row = 0; row < ROW_COUNT; row++) {
        digitalWrite(ROW_PINS[row], LOW);
        delayMicroseconds(5);

        for (uint8_t col = 0; col < COL_COUNT; col++) {
            const uint8_t index = MATRIX_MAP[row][col];

            // Assumes the diode direction allows the
            // pressed switch to pull the column LOW.
            rawSwitches[index] =
                (digitalRead(COL_PINS[col]) == LOW);
        }

        digitalWrite(ROW_PINS[row], HIGH);
    }
}

// ============================================================
// DEBOUNCING
// ============================================================

void updateDebounce() {
    const uint32_t now = millis();

    for (uint8_t i = 0; i < SWITCH_COUNT; i++) {
        if (rawSwitches[i] != candidateSwitches[i]) {
            candidateSwitches[i] = rawSwitches[i];
            candidateChangedAt[i] = now;
        }

        if (
            candidateSwitches[i] != switchStates[i] &&
            now - candidateChangedAt[i] >= DEBOUNCE_MS
        ) {
            switchStates[i] = candidateSwitches[i];
        }
    }
}

// ============================================================
// CONTROLLER MAPPING
// ============================================================

void updateControllerState() {
    // SW1 = Up, SW2 = Left, SW3 = Down, SW4 = Right.
    // Opposite directions cancel each other.

    stickUp = switchStates[0] && !switchStates[2];
    stickLeft = switchStates[1] && !switchStates[3];
    stickDown = switchStates[2] && !switchStates[0];
    stickRight = switchStates[3] && !switchStates[1];

    // SW5 = L2, SW6 = R2, SW7 = R1, SW8 = L1.
    buttonL2 = switchStates[4];
    buttonR2 = switchStates[5];
    buttonR1 = switchStates[6];
    buttonL1 = switchStates[7];
}

// ============================================================
// SERIAL OUTPUT
// ============================================================

void sendControllerState() {
    // Format:
    // STATE,UP,LEFT,DOWN,RIGHT,L1,L2,R1,R2
    // 0 = released, 1 = pressed.

    Serial.print("STATE,");

    Serial.print(stickUp);
    Serial.print(',');

    Serial.print(stickLeft);
    Serial.print(',');

    Serial.print(stickDown);
    Serial.print(',');

    Serial.print(stickRight);
    Serial.print(',');

    Serial.print(buttonL1);
    Serial.print(',');

    Serial.print(buttonL2);
    Serial.print(',');

    Serial.print(buttonR1);
    Serial.print(',');

    Serial.println(buttonR2);
}

// ============================================================
// OLED INITIALIZATION
// ============================================================

void initializeOLED() {
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

    oledReady = display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
    );

    if (!oledReady) {
        return;
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    display.println("FRC 5719 TITAN PAD");
    display.println("Initializing...");
    display.display();
}

// ============================================================
// OLED UPDATE
// ============================================================

void updateOLED() {
    if (!oledReady) {
        return;
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("TITAN PAD | FRC 5719");

    display.setCursor(0, 10);
    display.print("U:");
    display.print(stickUp);
    display.print(" L:");
    display.print(stickLeft);
    display.print(" D:");
    display.print(stickDown);
    display.print(" R:");
    display.print(stickRight);

    display.setCursor(0, 20);
    display.print("L1:");
    display.print(buttonL1);
    display.print(" L2:");
    display.print(buttonL2);
    display.print(" R1:");
    display.print(buttonR1);
    display.print(" R2:");
    display.print(buttonR2);

    display.display();
}
