#include "../../config.h"
#include <Arduino.h>

namespace Utils {

    unsigned long speedStart = 0;
    unsigned long speedEnd = 0;
    float currentSpeed = 0.0;

    float speedMeasure(unsigned long startTime, unsigned long endTime, float distance) {

        unsigned long deltaTime = endTime - startTime;

        if (deltaTime > 0) {
            return distance / (deltaTime / 1000.0);
        } else {
            return 0.0;
        }
    }
}
