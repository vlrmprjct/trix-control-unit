#pragma once

#include <Arduino.h>
#include <EEPROM.h>

namespace Utils {
    extern unsigned long speedStart;
    extern unsigned long speedEnd;
    extern float currentSpeed;

    float speedMeasure(unsigned long startTime, unsigned long endTime, float distance);
    float scaleSpeed(float cmPerSec);
    void setPrescalerTimers(uint8_t scale = 0x03);
    void debugEEPROM(int from = 0, int to = EEPROM.length());
    void clearEEPROM(uint8_t value = 0xFF);
    String getDirectionAnimation(int dir, unsigned long interval);
}
