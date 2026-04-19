# Model Railway Controller

This Arduino project is a **hybrid between analog train operation and digital control**. It controls a model railway with multiple stations, turnouts, motors, and an LCD display. It supports speed measurement, block section management, turnout control, and the display of operational data.

## Features

- **Motor control** for 4 independent zones (ZONE_A, ZONE_B, ZONE_C, ZONE_D)
- **Block section control** with automatic train separation (Zone B and Zone C blocking)
- **Speed measurement** (cm/s and km/h, scaled for 1:160)
- **Turnout control** via buttons and servos (11 turnouts)
- **Status display** on a 20x4 LCD
- **EEPROM storage** of track states and relay configuration
- **Modular structure** (Core, Controls, Display, Operation, Utils)

## Hardware Requirements

- Arduino Mega compatible board
- MCP4261XX digital potentiometers (2x dual-pot chips for 4 zones, soft SPI)
- PCA9685 PWM servo driver (I2C) for turnout servos
- 74HC165 shift registers for button and reed switch input
- 74HC595 shift registers for relay output
- Reed switches for track monitoring
- 20x4 LCD (HD44780 compatible)
- Rotary encoders (2x, interrupt-driven) for speed control per zone
- Buttons for manual track control
- Relays for track section switching

## Setup & Wiring

All relevant pins are documented in `src/core/config.h` and can be adjusted there.

## Operation

- **Buttons** toggle track power and switch turnout positions.
- **Reed switches** trigger block section logic and measure speed.
- **Rotary encoders** control motor speed per zone.
- **LCD** displays selected/powered tracks, encoder values, and speed in km/h.

## Code Structure

```
code/
  code.ino              – Main program (setup, loop)
  src/
    core/               – Configuration, state, enums, initialization
      config.h          – Pin assignments, constants, firmware version
      init.h            – Hardware initialization
      naming.h          – Enums for zones, buttons, reeds, turnouts
      state.h           – Track state struct and route data
    controls/           – Hardware driver modules
      buttonControl     – 74HC165 button input with edge detection
      encoderControl    – Interrupt-driven rotary encoders
      lcdControl        – LCD print with caching and RTL support
      motorControl      – MCP4261XX digital potentiometer control
      reedControl       – 74HC165 reed switch input
      relayControl      – 74HC595 relay output
      servoControl      – PCA9685 turnout servo control
    display/            – LCD layout definitions
      trackDisplay.h    – Track display slot table
    operation/          – Train operation logic
      blockControl      – Zone C blocking and BBF release logic
    utils/              – Utility functions
      debug             – Serial state and EEPROM diff output
      eeprom            – EEPROM load/save with versioned schema
      utils             – Speed measurement, scale conversion, timer
```

## Versioning

The firmware version (`FIRMWARE_VERSION` in `src/core/config.h`) is displayed on the LCD at startup. Versioning follows [SemVer](https://semver.org/).

## Notes

- Up to 3 trains can run simultaneously on separate zones (ZONE_A, B, C)
- ZONE_D is reserved for future expansion
- Soft SPI is used for motor control (MCP4261XX digital potentiometers)
- The minimum driving speed is limited by the motor characteristics
- The speed is converted to scale, but may be higher than prototype speed due to technical constraints
