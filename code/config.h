#pragma once

// EEPROM ADDRESS CONSTANTS
constexpr int EEPROM_ROUTE = 0;
constexpr int EEPROM_ACTIVE = 100;

// LCD DOT MATRIX
const int LCD_RS  = 34; // COMMON PIN FOR LCD1, LCD2, LCD3
const int LCD_EN  = 36; // COMMON PIN FOR LCD1, LCD2, LCD3
const int LCD1_D4 = 38;
const int LCD1_D5 = 40;
const int LCD1_D6 = 42;
const int LCD1_D7 = 44;

// NFC RC522 RFID MODULE
const int NFC_SDA = 53; // SS PIN | PIN 53
const int NFC_RST = 49; // RST PIN | PIN 49

// MOTOR MDL MAIN
const int MOTOR_MAIN_1 = 6; // BLUE  | PIN 6
const int MOTOR_MAIN_2 = 7; // WHITE | PIN 7

// MOTOR MDL HBF1
const int MOTOR_HBF1_1 = 2; // BLUE  | PIN 8
const int MOTOR_HBF1_2 = 3; // WHITE | PIN 9

// MOTOR MDL HBF2
const int MOTOR_HBF2_1 = 4; // BLUE  | PIN 8
const int MOTOR_HBF2_2 = 5; // WHITE | PIN 5

// MANUAL BUTTON CTRL
const int BTN_DATA   = 22;  // BLUE   | PIN 22
const int BTN_CLOCK  = 23;  // YELLOW | PIN 23
const int BTN_LATCH  = 24;  // GREEN  | PIN 24

// TRACK REED CTRL
const int REED_DATA  = 41;  // BLUE   | PIN 41
const int REED_CLOCK = 43;  // YELLOW | PIN 43
const int REED_LATCH = 45;  // GREEN  | PIN 45
const int REED_COUNT = 16;  // REED COUNT

// ENCODER MOTOR MDL MAIN
const int ENC_MAIN_1_CLOCK = 18; // GREEN  | PIN 18 | INTERRUPT
const int ENC_MAIN_1_DT    = 28; // YELLOW | PIN 28
inline int ENC_MAIN_1_CLOCK_STATE;
inline volatile int ENC_MAIN_1_VALUE = 0;

// SERVO MODULE [ NOT USED SOMEWHERE, JUST FOR PINOUT ]
// const int SERVO_SDA = 20;             // GREEN | PIN 20 SDA
// const int SERVO_SCL = 21;             // BLUE  | PIN 21 SCL
// const int SERVO_VOLTAGE_INTERNAL = 5; // YELLOW | 5V VOLTAGE PIN
// const int SERVO_VOLTAGE_EXTERNAL = 5; // RED | 5V EXTERNAL VOLTAGE

// RELAIS 74HC595 PINS
const int RELAY_LATCH = 39; // YELLOW | PIN 39 | ST_CP
const int RELAY_DATA  = 37; // GREEN  | PIN 37 | DS
const int RELAY_CLOCK = 35; // BLUE   | PIN 35 | SH_CP

// RELAY COUNT
const int RELAY_COUNT = 24; // RELAY COUNT
