# Model Railway Controller

This Arduino project is a **hybrid between analog train operation and digital control**. It controls a model railway with multiple stations, turnouts, motors, and an LCD display. It supports speed measurement, smooth braking, turnout control, and the display of operational data.

## Features

- **Motor control** for main track and stations (HBF1, HBF2, HBF3)
- **Smooth braking** when passing reed contacts
- **Speed measurement** (cm/s and km/h, scaled for 1:160)
- **Turnout control** via buttons and servos
- **Status display** on a 20x4 LCD
- **EEPROM storage** of the last station states
- **Modular structure** (Controls, Utils, Config, Naming)

## Hardware Requirements

- Arduino-Mega compatible board
- Motor drivers, DRV8871, for the trains
- Servos for the turnouts
- Reed switches for track monitoring
- 20x4 LCD (HD44780 compatible)
- Buttons for manual control
- Relays for additional switching tasks

## Setup & Wiring

All relevant pins are documented in `config.h` and can be adjusted there.

## Operation

- **Buttons** switch the turnout positions.
- **Reed switches** measure speed and trigger braking.
- **LCD** displays current speed, encoder value, km/h, percent, and active station.

## Code Structure

- `code.ino` – Main program (setup, loop)
- `config.h` – Pin assignments, global settings, states
- `src/controls/` – Control modules for motor, servo, LCD, buttons, reed, relay
- `src/utils/` – Utility functions (e.g., speed, timer)
- `naming.h` – Descriptive names for IDs and turnouts, buttons, reed

## Example: Smooth Braking

When passing reed 7, braking is initiated. The train continues at minimum speed until reed 8 is reached, then stops.

## Versioning

The firmware version is displayed on the LCD at startup. Versioning follows [SemVer](https://semver.org/).

## Notes

- The minimum driving speed is limited by the motor characteristics.
- The speed is converted to scale, but may be higher than prototype speed due to technical constraints.
