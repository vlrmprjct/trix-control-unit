#pragma once

// EEPROM ADDRESS CONSTANTS
constexpr int EEPROM_ROUTE  = 0;
constexpr int EEPROM_ACTIVE = 500;
constexpr int EEPROM_RELAY  = 1000;

// LCD DOT MATRIX
const int LCD_RS  = 34; // COMMON PIN FOR LCD1, LCD2, LCD3
const int LCD_EN  = 36; // COMMON PIN FOR LCD1, LCD2, LCD3
const int LCD1_D4 = 38;
const int LCD1_D5 = 40;
const int LCD1_D6 = 42;
const int LCD1_D7 = 44;

const int LCD_RST = 27;

// NFC RC522 RFID MODULE
const int NFC_SDA = 53; // SS PIN // WHITE
const int NFC_RST = 49; // RST PIN // BLUE
// BLUE   = 51
// YELLOW = 52
// GREEN  = 50

// MOTOR CTRL PSU MCP4261XX
const int DIGIPOT_MOSI = 33; // YELLOW
const int DIGIPOT_SCK  = 32; // ORANGE
const int DIGIPOT_CS   = 31; // PURPLE

// MANUAL BUTTON CTRL
const int BTN_DATA   = 22;  // PURPLE
const int BTN_CLOCK  = 23;  // YELLOW
const int BTN_LATCH  = 24;  // GREEN

// TRACK REED CTRL
const int REED_DATA  = 41;  // PURPLE
const int REED_CLOCK = 43;  // YELLOW
const int REED_LATCH = 45;  // ORANGE

// REED COUNT
const int REED_COUNT = 16;  // REED COUNT

// ENCODER PRIMARY
const int ENC_ZONE_A_CLK = 18; // GREEN  | INTERRUPT
const int ENC_ZONE_A_DT  = 28; // YELLOW
inline int ENC_ZONE_A_CLK_STATE;
inline volatile int ENC_ZONE_A = 0;

// ENCODER SECONDARY
const int ENC_ZONE_B_CLK = 19; // GREEN  | INTERRUPT
const int ENC_ZONE_B_DT  = 29; // YELLOW
inline int ENC_ZONE_B_CLK_STATE;
inline volatile int ENC_ZONE_B = 0;

// SERVO MODULE [ NOT USED SOMEWHERE, JUST FOR PINOUT ]
// const int SERVO_SDA = 20;             // PURPLE
// const int SERVO_SCL = 21;             // YELLOW
// const int SERVO_VOLTAGE_INTERNAL = 5; // BROWN | 5V VOLTAGE PIN
// const int SERVO_VOLTAGE_EXTERNAL = 5; // RED | 5V EXTERNAL VOLTAGE

// RELAIS 74HC595 PINS
const int RELAY_LATCH = 39; // YELLOW
const int RELAY_DATA  = 37; // ORANGE
const int RELAY_CLOCK = 35; // PURPLE

// RELAY COUNT
const int RELAY_COUNT = 24;
