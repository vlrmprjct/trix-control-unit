#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// #pragma once

namespace Utils {
    extern unsigned long speedStart;
    extern unsigned long speedEnd;
    extern float currentSpeed;

    float speedMeasure(unsigned long startTime, unsigned long endTime, float distance);
}

#endif
