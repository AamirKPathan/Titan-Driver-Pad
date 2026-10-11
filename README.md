# FRC 5719 Titan Driver Pad
![Final Product](<Assets/Titan Driver Pad Images/October 11, 2026/Final Product.png>)

A custom PCB-based button controller project for FRC 5719. The controller uses a Seeed Studio XIAO ESP32-C3, eight Cherry MX-style mechanical switches, and a small OLED display.

**Enclosure:** Bottom case only. There is no top case or cover.

## Project Status

- [x] Firmware builds successfully with PlatformIO.
- [x] OLED and graphics libraries installed.
- [ ] Assemble and test physical hardware.
- [ ] Verify all eight switches and OLED.
- [ ] Develop the PC-side virtual gamepad bridge.
- [ ] Test compatibility with the intended FRC Driver Station setup.

## Hardware

| Component | Details |
|---|---|
| Microcontroller | Seeed Studio XIAO ESP32-C3 |
| Switches | 8 Cherry MX-style mechanical switches |
| Display | 0.91-inch SSD1306 OLED, 128 × 32, I2C |
| PCB design software | KiCad |
| Firmware environment | VS Code + PlatformIO |
| Enclosure | Bottom case only; no top case |

## Controls

| Switch | Intended Input |
|---|---|
| SW1 | Left stick up (W / 1) |
| SW2 | Left stick left (A / 2) |
| SW3 | Left stick down (S / 3) |
| SW4 | Left stick right (D / 4) |
| SW5 | L2 |
| SW6 | R2 |
| SW7 | R1 |
| SW8 | L1 |

Controls are intended to remain active while a switch is held and deactivate when released. Opposite directions cancel each other out.

## Pin Assignments

### Switch Matrix

| Signal | XIAO Pin Label | GPIO |
|---|---|---|
| Row 1 | D0 / A0 | 2 |
| Row 2 | D1 / A1 | 3 |
| Row 3 | D2 / A2 | 4 |
| Row 4 | D3 / A3 | 5 |
| Column 1 | D6 / TX | 8 |
| Column 2 | D7 / RX | 9 |

### Matrix Layout

|  | Column 1 (GPIO 8) | Column 2 (GPIO 9) |
|---|---|---|
| Row 1 (GPIO 2) | SW1 | SW5 |
| Row 2 (GPIO 3) | SW2 | SW6 |
| Row 3 (GPIO 4) | SW3 | SW7 |
| Row 4 (GPIO 5) | SW4 | SW8 |

**Note:** The physical matrix wiring and diode orientation have not yet been verified. Confirm them against the final KiCad schematic and PCB when the hardware arrives.

### OLED Display

| OLED Signal | XIAO Pin Label | GPIO |
|---|---|---|
| SDA | D4 / A4 / SDA | 6 |
| SCL | D5 / A5 / SCL | 7 |

The firmware currently uses I2C address `0x3C`. Some OLED modules use `0x3D`.

## Firmware

The firmware is developed using VS Code and PlatformIO with the Arduino framework for ESP32-C3.

### Build Firmware

Open a terminal in the `Firmware` directory and run:

```bash
pio run
```

### Upload Firmware

Once the hardware arrives, connect the XIAO ESP32-C3 through USB and run:

```bash
pio run --target upload
```

### Open Serial Monitor

```bash
pio device monitor
```

The firmware is intended to output controller states in the following format:

```text
STATE,UP,LEFT,DOWN,RIGHT,L1,L2,R1,R2
```

Each state is represented by `0` (inactive) or `1` (active).

## Software Dependencies

- VS Code
- PlatformIO
- Arduino framework for ESP32-C3
- Adafruit GFX Library
- Adafruit SSD1306 Library

## Current Limitations

- Firmware compilation has succeeded, but the firmware has not yet been tested on physical hardware.
- Switch scanning, debounce behavior, diode orientation, and matrix wiring still require verification.
- OLED operation must be confirmed on the assembled PCB.
- The PC-side virtual gamepad bridge is still to be developed and tested.
- Compatibility with the intended FRC Driver Station setup has not yet been confirmed.

## Hardware Testing Checklist

Once the hardware arrives:

- [ ] Inspect the PCB and verify component orientation.
- [ ] Check power, ground, and possible shorts before connecting USB.
- [ ] Upload the firmware.
- [ ] Confirm that the OLED initializes.
- [ ] Test each switch individually.
- [ ] Verify that every switch activates the correct control.
- [ ] Confirm opposite directions cancel each other.
- [ ] Confirm controls deactivate when switches are released.
- [ ] Test the PC-side virtual gamepad bridge.
- [ ] Verify the intended FRC Driver Station workflow.

## Project Structure

The project may be organized as follows:

```text
Titan Driver Pad/
├── Firmware/
│   ├── platformio.ini
│   └── src/
│       └── main.cpp
├── Hardware/
│   └── KiCad project files
└── README.md
```

Adjust the folder names in this section to match the actual project structure.

## Credits

Developed for FRC Team 5719 — Titan.