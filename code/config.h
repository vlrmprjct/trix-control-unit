#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// MOTOR MDL 1.1
const int MTR_MDL_1_CTRL = A0;
const int MTR_MDL_1_ENA  =  2;
const int MTR_MDL_1_IN1  = 22;
const int MTR_MDL_1_IN2  = 23;

// MOTOR MDL 1.2
const int MTR_MDL_2_CTRL = A1;
const int MTR_MDL_2_ENB  =  3;
const int MTR_MDL_2_IN3  = 24;
const int MTR_MDL_2_IN4  = 25;

// RELAIS 74HC595 PINS
const int RELAY_LATCH =  8; // (ST_CP) YELLOW
const int RELAY_DATA =   9; // (DS) GREEN
const int RELAY_CLOCK = 10; // (SH_CP) BLUE

// RELAY COUNT
const int RELAY_COUNT = 24; // Anzahl der Relais

// MOTOR DEAD ZONE
const int DEAD_RANGE = 15;
const int DEAD_THRESHOLD = 5;

#endif
