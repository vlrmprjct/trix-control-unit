# Arduino Mega Pin Assignment – Trix Control Unit

## Overview

This file documents the complete pin assignment of the Arduino Mega for the model railway controller.

---

## Dashboard Control Unit

### LCD Dot Matrix Display
| Pin Number | Function | Description |
|------------|----------|-------------|
| 34 | LCD_RS | Register Select (shared for LCD1, LCD2, LCD3) |
| 36 | LCD_EN | Enable (shared for LCD1, LCD2, LCD3) |
| 38 | LCD1_D4 | Data line 4 |
| 40 | LCD1_D5 | Data line 5 |
| 42 | LCD1_D6 | Data line 6 |
| 44 | LCD1_D7 | Data line 7 |
| 27 | LCD_RST | Reset line |

**Wire colors:**
- *(not documented)*

---

### Button Control (74HC165 Shift Register)
| Pin Number | Function | Description | Wire Color | Mode |
|------------|----------|-------------|------------|------|
| 22 | BTN_DATA | Data input from shift register | LILA (Purple) | INPUT |
| 23 | BTN_CLOCK | Clock line | GELB (Yellow) | OUTPUT |
| 24 | BTN_LATCH | Latch line | GRÜN (Green) | OUTPUT |

**Connection:** 16 dashboard buttons via 74HC165 shift register

---

## Track Unit

### Motor Control – Digital Potentiometer (MCP4261XX)
| Pin Number | Function | Description | Wire Color | Mode |
|------------|----------|-------------|------------|------|
| 51 | MOSI | Hardware SPI Data Out | GELB (Yellow) | OUTPUT |
| 52 | SCK | Hardware SPI Clock | ORANGE | OUTPUT |
| 31 | DIGIPOT_CS | Chip Select 1 (ZONE_A, ZONE_B) | LILA (Purple) | OUTPUT |
| 30 | DIGIPOT2_CS | Chip Select 2 (ZONE_C, ZONE_D) | LILA (Purple) | OUTPUT |

**Connection:** 2x MCP4261XX dual potentiometers for 4-zone motor control via Hardware SPI

---

### Reed Switch Control (74HC165 Shift Register)
| Pin Number | Function | Description | Wire Color | Mode |
|------------|----------|-------------|------------|------|
| 41 | REED_DATA | Data input – REED1-Board (IC1+IC2) | LILA (Purple) | INPUT |
| 46 | REED2_DATA | Data input – REED2-Board (IC3+IC4) | LILA (Purple) | INPUT |
| 43 | REED_CLOCK | Clock line (shared) | GELB (Yellow) | OUTPUT |
| 45 | REED_LATCH | Latch line (shared) | ORANGE | OUTPUT |

**Connection:** 32 reed sensors across 2 boards, CLK and LATCH shared

---

### Encoder Zone A (mounted left)
| Pin Number | Function | Description | Wire Color | Mode |
|------------|----------|-------------|------------|------|
| 18 | ENC_ZONE_A_CLK | Clock signal | GRÜN (Green) | INPUT_PULLUP + **INTERRUPT** |
| 28 | ENC_ZONE_A_DT | Data signal | GELB (Yellow) | INPUT_PULLUP |

**Connection:** Rotary encoder for speed control Zone A
**Value range:** 70 - 255

---

### Encoder Zone B (mounted right)
| Pin Number | Function | Description | Wire Color | Mode |
|------------|----------|-------------|------------|------|
| 19 | ENC_ZONE_B_CLK | Clock signal | GRÜN (Green) | INPUT_PULLUP + **INTERRUPT** |
| 29 | ENC_ZONE_B_DT | Data signal | GELB (Yellow) | INPUT_PULLUP |

**Connection:** Rotary encoder for speed control Zone B
**Value range:** 70 - 255

---

### Servo Module (I²C) – Turnout Control
| Pin Number | Function | Description | Wire Color | Bus |
|------------|----------|-------------|------------|-----|
| 20 | SERVO_SDA | I²C data line | LILA (Purple) | I²C |
| 21 | SERVO_SCL | I²C clock line | GELB (Yellow) | I²C |
| 5V | SERVO_VOLTAGE_INTERNAL | Internal 5V supply | BRAUN (Brown) | - |
| 5V | SERVO_VOLTAGE_EXTERNAL | External 5V supply | ROT (Red) | - |

**Connection:** Adafruit PWM Servo Driver (PCA9685) via I²C
**Usage:** Control of model railway turnouts

---

### Relay Control (74HC595 Shift Register)
| Pin Number | Function | Description | Wire Color | Mode |
|------------|----------|-------------|------------|------|
| 39 | RELAY_LATCH | Latch line | GELB (Yellow) | OUTPUT |
| 37 | RELAY_DATA | Data output | ORANGE | OUTPUT |
| 35 | RELAY_CLOCK | Clock line | LILA (Purple) | OUTPUT |

**Connection:** 24 relays via 74HC595 shift register
**Usage:** Switching of track sections, turnouts, etc.

---

## Pin Overview by Number

### Digital Pins (0–53)

| Pin | Function | Module | Note |
|-----|----------|--------|------|
| 0 | RX0 | - | Not used (Serial) |
| 1 | TX0 | - | Not used (Serial) |
| 2 | - | - | Not used (INT4) |
| 3 | - | - | Not used (PWM, INT5) |
| 4 | - | - | Not used (PWM) |
| 5 | - | - | Not used (PWM) |
| 6 | - | - | Not used (PWM) |
| 7 | - | - | Not used (PWM) |
| 8 | - | - | Not used (PWM) |
| 9 | - | - | Not used (PWM) |
| 10 | - | - | Not used (PWM) |
| 11 | - | - | Not used (PWM) |
| 12 | - | - | Not used (PWM) |
| 13 | - | - | Not used (PWM, LED_BUILTIN) |
| 14 | TX3 | - | Not used (Serial3) |
| 15 | RX3 | - | Not used (Serial3) |
| 16 | TX2 | - | Not used (Serial2) |
| 17 | RX2 | - | Not used (Serial2) |
| 18 | ENC_ZONE_A_CLK | Encoder Zone A | **INT5 - INTERRUPT** |
| 19 | ENC_ZONE_B_CLK | Encoder Zone B | **INT4 - INTERRUPT** |
| 20 | SERVO_SDA | Servo Module | **I²C SDA** |
| 21 | SERVO_SCL | Servo Module | **I²C SCL** |
| 22 | BTN_DATA | Button Control | INPUT |
| 23 | BTN_CLOCK | Button Control | OUTPUT |
| 24 | BTN_LATCH | Button Control | OUTPUT |
| 25 | - | - | Not used |
| 26 | - | - | Not used |
| 27 | LCD_RST | LCD Display | INPUT (Reset) |
| 28 | ENC_ZONE_A_DT | Encoder Zone A | INPUT_PULLUP |
| 29 | ENC_ZONE_B_DT | Encoder Zone B | INPUT_PULLUP |
| 30 | DIGIPOT2_CS | Motor Control | OUTPUT (SPI CS2) |
| 31 | DIGIPOT_CS | Motor Control | OUTPUT (SPI CS1) |
| 32 | - | - | Not used |
| 33 | - | - | Not used |
| 34 | LCD_RS | LCD Display | Register Select |
| 35 | RELAY_CLOCK | Relay Control | OUTPUT |
| 36 | LCD_EN | LCD Display | Enable |
| 37 | RELAY_DATA | Relay Control | OUTPUT |
| 38 | LCD1_D4 | LCD Display | Data line 4 |
| 39 | RELAY_LATCH | Relay Control | OUTPUT |
| 40 | LCD1_D5 | LCD Display | Data line 5 |
| 41 | REED_DATA | Reed Control | INPUT |
| 42 | LCD1_D6 | LCD Display | Data line 6 |
| 43 | REED_CLOCK | Reed Control | OUTPUT |
| 44 | LCD1_D7 | LCD Display | Data line 7 |
| 45 | REED_LATCH | Reed Control | OUTPUT |
| 46 | REED2_DATA | Reed Control | INPUT |
| 47 | - | - | Not used (PWM) |
| 48 | - | - | Not used (PWM) |
| 49 | - | - | Not used (PWM) |
| 50 | MISO | - | Not used (Hardware SPI) |
| 51 | MOSI | Motor Control | OUTPUT (Hardware SPI) |
| 52 | SCK | Motor Control | OUTPUT (Hardware SPI) |
| 53 | SS | - | Not used (Hardware SPI) |

### Analog Pins (A0–A15)

| Pin | Function | Module | Note |
|-----|----------|--------|------|
| A0 | - | - | Not used |
| A1 | - | - | Not used |
| A2 | - | - | Not used |
| A3 | - | - | Not used |
| A4 | - | - | Not used |
| A5 | - | - | Not used |
| A6 | - | - | Not used |
| A7 | - | - | Not used |
| A8 | - | - | Not used |
| A9 | - | - | Not used |
| A10 | - | - | Not used |
| A11 | - | - | Not used |
| A12 | - | - | Not used |
| A13 | - | - | Not used |
| A14 | - | - | Not used |
| A15 | - | - | Not used |

### Power & Special Pins

| Pin | Function | Description |
|-----|----------|-------------|
| GND | Ground | Ground (multiple pins) |
| 5V | 5V Power | 5V power supply (multiple pins) |
| 3.3V | 3.3V Power | 3.3V power supply (max. 50mA) |
| VIN | Input Voltage | External power supply (7-12V recommended) |
| IOREF | I/O Reference | Reference voltage for shield compatibility |
| AREF | Analog Reference | Reference voltage for analog inputs |
| RESET | Reset | Reset pin (LOW = reset) |

---

## Communication Protocols Used

### SPI (Serial Peripheral Interface)
- **Motor Control (MCP4261XX):** Pins 30, 31, 51, 52
  - Hardware SPI (MOSI=51, SCK=52, CS1=31, CS2=30)

### I²C (Inter-Integrated Circuit)
- **Servo Control (PCA9685):** Pins 20, 21
  - Standard Arduino Wire Library

### Shift Register (74HC165 / 74HC595)
- **Button Input (74HC165):** Pins 22, 23, 24
- **Reed Input (74HC165):** Pins 41, 43, 45
- **Relay Output (74HC595):** Pins 35, 37, 39

---

## Interrupt Pins

The Arduino Mega supports external interrupts on the following used pins:

| Pin | Int. No. | Usage |
|-----|----------|-------|
| 18 | Interrupt 5 | Encoder Zone A CLK |
| 19 | Interrupt 4 | Encoder Zone B CLK |
