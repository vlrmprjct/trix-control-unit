#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// TRACK INITIALIZATION
inline volatile bool HBF1 = true;
inline volatile bool HBF2 = false;
inline volatile bool HBF3 = false;

// LCD DOT MATRIX PINS
const int LCD_RS = 34; // COMMON PIN FOR LCD1, LCD2, LCD3
const int LCD_EN = 36; // COMMON PIN FOR LCD1, LCD2, LCD3
const int LCD1_D4 = 38;
const int LCD1_D5 = 40;
const int LCD1_D6 = 42;
const int LCD1_D7 = 44;

// MOTOR MDL 1 (MAIN)
const int MOTOR_IN1 = 6; // BLUE  | PIN 6
const int MOTOR_IN2 = 7; // WHITE | PIN 7

// MOTOR MDL 1 (HBF2)
const int MOTOR_HBF2_1 = 4; // BLUE  | PIN 4
const int MOTOR_HBF2_2 = 5; // WHITE | PIN 5

// MOTOR MDL 1 (HBF1)
const int MOTOR_HBF1_1 = 2; // BLUE  | PIN 2
const int MOTOR_HBF1_2 = 3; // WHITE | PIN 3

// BUTTON CTRL
const int BTN_DATA   = 46;  // BLUE   | PIN 46
const int BTN_CLOCK  = 48;  // YELLOW | PIN 48
const int BTN_LATCH  = 50;  // GREEN  | PIN 50

// ENCODER MOTOR MDL 1
const int ENC_MAIN_1_CLK = 18; // GREEN  | PIN 18 | INTERRUPT
const int ENC_MAIN_1_DT  = 28; // YELLOW | PIN 28
inline int ENC_MAIN_1_CLK_STATE;
inline int ENC_MAIN_1_DT_STATE;
inline volatile int ENC_MAIN_1_VALUE = 0;

// SERVO MODULE [ NOT USED SOMEWHERE, JUST FOR PINOUT ]
// const int SERVO_SDA = 20;    // GREEN | PIN 20 SDA
// const int SERVO_SCL = 21;    // BLUE  | PIN 21 SCL
// const int SERVO_VOLTAGE_INTERNAL = 5; // YELLOW | 5V VOLTAGE PIN
// const int SERVO_VOLTAGE_EXTERNAL = 5; // RED | 5V EXTERNAL VOLTAGE

// RELAIS 74HC595 PINS
const int RELAY_LATCH = 39; // YELLOW | PIN 39 | ST_CP
const int RELAY_DATA  = 37; // GREEN  | PIN 37 | DS
const int RELAY_CLOCK = 35; // BLUE   | PIN 35 | SH_CP

// RELAY COUNT
const int RELAY_COUNT = 24; // RELAY COUNT

#endif
