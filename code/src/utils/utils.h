#pragma once

#include <Arduino.h>

namespace Utils {
    extern unsigned long speedStart;
    extern unsigned long speedEnd;
    extern float currentSpeed;

    float speedMeasure(unsigned long startTime, unsigned long endTime, float distance);
    float scaleSpeed(float cmPerSec);
    void setPrescalerTimers(uint8_t scale = 0x03);
    String getDirectionAnimation(int dir, unsigned long interval);
}
