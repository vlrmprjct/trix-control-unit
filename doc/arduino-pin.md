# Arduino Mega Pinbelegung - Trix Control Unit

## Übersicht

Diese Datei dokumentiert die komplette Pin-Belegung des Arduino Mega für die Modellbahn-Steuerung.

---

## Dashboard Control Unit

### LCD Dot Matrix Display
| Pin-Nummer | Funktion | Beschreibung |
|------------|----------|--------------|
| 34 | LCD_RS | Register Select (gemeinsam für LCD1, LCD2, LCD3) |
| 36 | LCD_EN | Enable (gemeinsam für LCD1, LCD2, LCD3) |
| 38 | LCD1_D4 | Datenleitung 4 |
| 40 | LCD1_D5 | Datenleitung 5 |
| 42 | LCD1_D6 | Datenleitung 6 |
| 44 | LCD1_D7 | Datenleitung 7 |
| 27 | LCD_RST | Reset-Leitung |

**Kabelfarben:**
- *(keine Angaben in Dokumentation)*

---

### Button Control (74HC165 Shift Register)
| Pin-Nummer | Funktion | Beschreibung | Kabelfarbe | Modus |
|------------|----------|--------------|------------|-------|
| 22 | BTN_DATA | Dateneingang vom Shift Register | LILA (Purple) | INPUT |
| 23 | BTN_CLOCK | Taktleitung | GELB (Yellow) | OUTPUT |
| 24 | BTN_LATCH | Latch-Leitung | GRÜN (Green) | OUTPUT |

**Anschluss:** 16 Dashboard-Buttons über 74HC165 Shift Register

---

## Track Unit (Gleissteuerung)

### Motor Control - Digital Potentiometer (MCP4261XX)
| Pin-Nummer | Funktion | Beschreibung | Kabelfarbe | Modus |
|------------|----------|--------------|------------|-------|
| 33 | DIGIPOT_MOSI | SPI Data Out | GELB (Yellow) | OUTPUT |
| 32 | DIGIPOT_SCK | SPI Clock | ORANGE | OUTPUT |
| 31 | DIGIPOT_CS | Chip Select | LILA (Purple) | OUTPUT |

**Anschluss:** Stromversorgungssteuerung über digitales Potentiometer

---

### Reed Switch Control (74HC165 Shift Register)
| Pin-Nummer | Funktion | Beschreibung | Kabelfarbe | Modus |
|------------|----------|--------------|------------|-------|
| 41 | REED_DATA | Dateneingang | LILA (Purple) | INPUT |
| 43 | REED_CLOCK | Taktleitung | GELB (Yellow) | OUTPUT |
| 45 | REED_LATCH | Latch-Leitung | ORANGE | OUTPUT |

**Anschluss:** 16 Reed-Sensoren zur Gleisbelegungserkennung

---

### Encoder Zone A (Links montiert)
| Pin-Nummer | Funktion | Beschreibung | Kabelfarbe | Modus |
|------------|----------|--------------|------------|-------|
| 18 | ENC_ZONE_A_CLK | Clock-Signal | GRÜN (Green) | INPUT_PULLUP + **INTERRUPT** |
| 28 | ENC_ZONE_A_DT | Data-Signal | GELB (Yellow) | INPUT_PULLUP |

**Anschluss:** Drehencoder für Geschwindigkeitssteuerung Zone A
**Wertebereich:** 70 - 255

---

### Encoder Zone B (Rechts montiert)
| Pin-Nummer | Funktion | Beschreibung | Kabelfarbe | Modus |
|------------|----------|--------------|------------|-------|
| 19 | ENC_ZONE_B_CLK | Clock-Signal | GRÜN (Green) | INPUT_PULLUP + **INTERRUPT** |
| 29 | ENC_ZONE_B_DT | Data-Signal | GELB (Yellow) | INPUT_PULLUP |

**Anschluss:** Drehencoder für Geschwindigkeitssteuerung Zone B
**Wertebereich:** 70 - 255

---

### Servo Module (I²C) - Weichensteuerung
| Pin-Nummer | Funktion | Beschreibung | Kabelfarbe | Bus |
|------------|----------|--------------|------------|-----|
| 20 | SERVO_SDA | I²C Datenleitung | LILA (Purple) | I²C |
| 21 | SERVO_SCL | I²C Taktleitung | GELB (Yellow) | I²C |
| 5V | SERVO_VOLTAGE_INTERNAL | Interne 5V Versorgung | BRAUN (Brown) | - |
| 5V | SERVO_VOLTAGE_EXTERNAL | Externe 5V Versorgung | ROT (Red) | - |

**Anschluss:** Adafruit PWM Servo Driver (PCA9685) über I²C
**Verwendung:** Steuerung von Modellbahn-Weichen

---

### Relay Control (74HC595 Shift Register)
| Pin-Nummer | Funktion | Beschreibung | Kabelfarbe | Modus |
|------------|----------|--------------|------------|-------|
| 39 | RELAY_LATCH | Latch-Leitung | GELB (Yellow) | OUTPUT |
| 37 | RELAY_DATA | Datenausgang | ORANGE | OUTPUT |
| 35 | RELAY_CLOCK | Taktleitung | LILA (Purple) | OUTPUT |

**Anschluss:** 24 Relais über 74HC595 Shift Register
**Verwendung:** Schaltung von Gleisabschnitten, Weichen, etc.

---

## Pin-Übersicht nach Nummern

### Digitale Pins (0-53)

| Pin | Funktion | Modul | Bemerkung |
|-----|----------|-------|-----------|
| 0 | RX0 | - | Nicht verwendet (Serial) |
| 1 | TX0 | - | Nicht verwendet (Serial) |
| 2 | - | - | Nicht verwendet (INT4) |
| 3 | - | - | Nicht verwendet (PWM, INT5) |
| 4 | - | - | Nicht verwendet (PWM) |
| 5 | - | - | Nicht verwendet (PWM) |
| 6 | - | - | Nicht verwendet (PWM) |
| 7 | - | - | Nicht verwendet (PWM) |
| 8 | - | - | Nicht verwendet (PWM) |
| 9 | - | - | Nicht verwendet (PWM) |
| 10 | - | - | Nicht verwendet (PWM) |
| 11 | - | - | Nicht verwendet (PWM) |
| 12 | - | - | Nicht verwendet (PWM) |
| 13 | - | - | Nicht verwendet (PWM, LED_BUILTIN) |
| 14 | TX3 | - | Nicht verwendet (Serial3) |
| 15 | RX3 | - | Nicht verwendet (Serial3) |
| 16 | TX2 | - | Nicht verwendet (Serial2) |
| 17 | RX2 | - | Nicht verwendet (Serial2) |
| 18 | ENC_ZONE_A_CLK | Encoder Zone A | **INT5 - INTERRUPT** |
| 19 | ENC_ZONE_B_CLK | Encoder Zone B | **INT4 - INTERRUPT** |
| 20 | SERVO_SDA | Servo Module | **I²C SDA** |
| 21 | SERVO_SCL | Servo Module | **I²C SCL** |
| 22 | BTN_DATA | Button Control | INPUT |
| 23 | BTN_CLOCK | Button Control | OUTPUT |
| 24 | BTN_LATCH | Button Control | OUTPUT |
| 25 | - | - | Nicht verwendet |
| 26 | - | - | Nicht verwendet |
| 27 | LCD_RST | LCD Display | INPUT (Reset) |
| 28 | ENC_ZONE_A_DT | Encoder Zone A | INPUT_PULLUP |
| 29 | ENC_ZONE_B_DT | Encoder Zone B | INPUT_PULLUP |
| 30 | - | - | Nicht verwendet |
| 31 | DIGIPOT_CS | Motor Control | OUTPUT (SPI CS) |
| 32 | DIGIPOT_SCK | Motor Control | OUTPUT (SPI SCK) |
| 33 | DIGIPOT_MOSI | Motor Control | OUTPUT (SPI MOSI) |
| 34 | LCD_RS | LCD Display | Register Select |
| 35 | RELAY_CLOCK | Relay Control | OUTPUT |
| 36 | LCD_EN | LCD Display | Enable |
| 37 | RELAY_DATA | Relay Control | OUTPUT |
| 38 | LCD1_D4 | LCD Display | Datenleitung 4 |
| 39 | RELAY_LATCH | Relay Control | OUTPUT |
| 40 | LCD1_D5 | LCD Display | Datenleitung 5 |
| 41 | REED_DATA | Reed Control | INPUT |
| 42 | LCD1_D6 | LCD Display | Datenleitung 6 |
| 43 | REED_CLOCK | Reed Control | OUTPUT |
| 44 | LCD1_D7 | LCD Display | Datenleitung 7 |
| 45 | REED_LATCH | Reed Control | OUTPUT |
| 46 | - | - | Nicht verwendet (PWM) |
| 47 | - | - | Nicht verwendet (PWM) |
| 48 | - | - | Nicht verwendet (PWM) |
| 49 | - | - | Nicht verwendet (PWM) |
| 50 | MISO | - | Nicht verwendet (Hardware SPI) |
| 51 | MOSI | - | Nicht verwendet (Hardware SPI) |
| 52 | SCK | - | Nicht verwendet (Hardware SPI) |
| 53 | SS | - | Nicht verwendet (Hardware SPI) |

### Analoge Pins (A0-A15)

| Pin | Funktion | Modul | Bemerkung |
|-----|----------|-------|-----------|
| A0 | - | - | Nicht verwendet |
| A1 | - | - | Nicht verwendet |
| A2 | - | - | Nicht verwendet |
| A3 | - | - | Nicht verwendet |
| A4 | - | - | Nicht verwendet |
| A5 | - | - | Nicht verwendet |
| A6 | - | - | Nicht verwendet |
| A7 | - | - | Nicht verwendet |
| A8 | - | - | Nicht verwendet |
| A9 | - | - | Nicht verwendet |
| A10 | - | - | Nicht verwendet |
| A11 | - | - | Nicht verwendet |
| A12 | - | - | Nicht verwendet |
| A13 | - | - | Nicht verwendet |
| A14 | - | - | Nicht verwendet |
| A15 | - | - | Nicht verwendet |

### Power & Spezial-Pins

| Pin | Funktion | Beschreibung |
|-----|----------|--------------|
| GND | Ground | Masse (mehrere Pins) |
| 5V | 5V Power | 5V Spannungsversorgung (mehrere Pins) |
| 3.3V | 3.3V Power | 3.3V Spannungsversorgung (max. 50mA) |
| VIN | Input Voltage | Externe Spannungsversorgung (7-12V empfohlen) |
| IOREF | I/O Reference | Referenzspannung für Shield-Kompatibilität |
| AREF | Analog Reference | Referenzspannung für analoge Eingänge |
| RESET | Reset | Reset-Pin (LOW = Reset)

---

## Verwendete Kommunikationsprotokolle

### SPI (Serial Peripheral Interface)
- **Motor Control (MCP4261XX):** Pins 31, 32, 33
  - Custom Software-SPI Implementation

### I²C (Inter-Integrated Circuit)
- **Servo Control (PCA9685):** Pins 20, 21
  - Standard Arduino Wire Library

### Shift Register (74HC165 / 74HC595)
- **Button Input (74HC165):** Pins 22, 23, 24
- **Reed Input (74HC165):** Pins 41, 43, 45
- **Relay Output (74HC595):** Pins 35, 37, 39

---

## Interrupt-Pins

Der Arduino Mega unterstützt externe Interrupts auf folgenden verwendeten Pins:

| Pin | Interrupt-Nr. | Verwendung |
|-----|---------------|------------|
| 18 | Interrupt 5 | Encoder Zone A CLK |
| 19 | Interrupt 4 | Encoder Zone B CLK |

---

## Modulzusammenfassung

| Modul | Anzahl | Steuerung über |
|-------|--------|----------------|
| Dashboard Buttons | 16 | 74HC165 Shift Register |
| Reed Switches | 16 | 74HC165 Shift Register |
| Relays | 24 | 74HC595 Shift Register |
| Encoder | 2 | Direkt (mit Interrupts) |
| LCD Displays | 1-3 | Parallel 4-Bit Mode |
| Servos | Variable | I²C PWM Driver |
| Motor PSU | 1 | SPI Digital Potentiometer |

---

## Hinweise

- **Interrupt-Pins 18 & 19** werden für Encoder-Signale verwendet und ermöglichen präzise Drehrichtungserkennung
- Alle **Shift Register** verwenden separate Clock/Latch/Data-Leitungen für saubere Signaltrennung
- **I²C-Bus** (Pins 20, 21) kann bei Bedarf für weitere Module erweitert werden
- **5V Logikpegel** werden durchgehend verwendet

---

*Generiert am: 08.03.2026*
*Projekt: Trix Control Unit - Modellbahn-Steuerung*
