#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <EEPROM.h>

// #pragma once

namespace Utils {
    extern unsigned long speedStart;
    extern unsigned long speedEnd;
    extern float currentSpeed;

    float speedMeasure(unsigned long startTime, unsigned long endTime, float distance);
    float scaleSpeed(float cmPerSec);
    void setPrescalerTimers(uint8_t scale = 0x03);
    void debugEEPROM(int from = 0, int to = EEPROM.length());
    }

#endif
