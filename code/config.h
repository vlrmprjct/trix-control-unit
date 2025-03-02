#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// MAIN SPEED PINS
const int MAIN_CTRL = A0;
const int ENA = 2;
const int IN1 = 22;
const int IN2 = 23;

// SECOND SPEED PINS
const int MAIN_CTRL = A1;
const int ENA = 3;
const int IN1 = 24;
const int IN2 = 25;

// RELAIS 74HC595 PINS
const int LATCH = 8; // Latch-Pin (ST_CP) VIOLET
const int CLOCK = 9; // Clock-Pin (SH_CP) BLUE
const int DATA = 10; // Data-Pin (DS) GREEN

// RELAY COUNT
const int RELAY_COUNT = 24; // Anzahl der Relais

// MOTOR DEAD ZONE
const int DEAD_RANGE = 15;
const int DEAD_THRESHOLD = 5;

#endif
