#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Mega Interrupt Pins
// Pin 2
// Pin 3
// Pin 18
// Pin 19
// Pin 20
// Pin 21

// MOTOR MDL 1.1
const int MOTOR_IN1 = 6;
const int MOTOR_IN2 = 7;

const int MTR_MDL_1_CTRL = A0;
const int MTR_MDL_1_ENA  =  2;
const int MTR_MDL_1_IN1  = 22;
const int MTR_MDL_1_IN2  = 23;

// MOTOR MDL 1.2
const int MTR_MDL_2_CTRL = A1;
const int MTR_MDL_2_ENB  =  3;
const int MTR_MDL_2_IN3  = 24;
const int MTR_MDL_2_IN4  = 25;

// ENCODER MOTOR MDL 1.1
const int ENC_MAIN_1_CLK = 18; // INTERRUPT !
const int ENC_MAIN_1_DT  = 28;
inline int ENC_MAIN_1_CLK_STATE;
inline int ENC_MAIN_1_DT_STATE;
inline volatile int ENC_MAIN_1_VALUE = 0;

// RELAIS 74HC595 PINS
const int RELAY_LATCH = 39; // (ST_CP) YELLOW // 39
const int RELAY_DATA  = 37; // (DS) GREEN // 37
const int RELAY_CLOCK = 35; // (SH_CP) BLUE // 35

// RELAY COUNT
const int RELAY_COUNT = 24; // Anzahl der Relais

// MOTOR DEAD ZONE
const int DEAD_RANGE = 15;
const int DEAD_THRESHOLD = 5;

#endif
