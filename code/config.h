#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// LCD DOT MATRIX PINS | LEAVING HERE FOR DOCUMENTATION
// const int LCD_SCL = 21; // YELLOW | PIN 21 | SCL
// const int LCD_SDA = 20; // GREEN  | PIN 20 | SDA

// MOTOR MDL 1
const int MOTOR_IN1 = 6; // YELLOW | PIN 6
const int MOTOR_IN2 = 7; // GREEN  | PIN 7

// ENCODER MOTOR MDL 1
const int ENC_MAIN_1_CLK = 18; // GREEN  | PIN 18 | INTERRUPT
const int ENC_MAIN_1_DT  = 28; // YELLOW | PIN 28
inline int ENC_MAIN_1_CLK_STATE;
inline int ENC_MAIN_1_DT_STATE;
inline volatile int ENC_MAIN_1_VALUE = 0;

// RELAIS 74HC595 PINS
const int RELAY_LATCH = 39; // YELLOW | PIN 39 | ST_CP
const int RELAY_DATA  = 37; // GREEN  | PIN 37 | DS
const int RELAY_CLOCK = 35; // BLUE   | PIN 35 | SH_CP

// RELAY COUNT
const int RELAY_COUNT = 24; // RELAY COUNT

#endif
