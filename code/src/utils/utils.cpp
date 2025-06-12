#include "../../config.h"
#include <Arduino.h>

namespace Utils {

    unsigned long speedStart = 0;
    unsigned long speedEnd = 0;
    float currentSpeed = 0.0;

    /**
     * Measures speed based on the time taken to cover a certain distance.
     *
     * @param startTime The time when the measurement started (in milliseconds).
     * @param endTime The time when the measurement ended (in milliseconds).
     * @param distance The distance covered during the measurement (in cm).
     * @return The speed in cm/s.
     */
    float speedMeasure(unsigned long startTime, unsigned long endTime, float distance) {

        unsigned long deltaTime = endTime - startTime;

        if (deltaTime > 0) {
            return distance / (deltaTime / 1000.0);
        } else {
            return 0.0;
        }
    }

    /**
     * Scales the speed from cm/s to km/h based on the model's scale.
     *
     * @param cmPerSec Speed in cm/s.
     * @return Speed in km/h.
     */
    float scaleSpeed(float cmPerSec) {
        // 1 cm/s in the model corresponds to (0.036 * scale) km/h in the original
        return cmPerSec * 0.036 * 160;
    }
}
