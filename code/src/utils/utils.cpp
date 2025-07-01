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

    /**
     * Sets the prescaler for the timers to control their speed.
     * The scale parameter determines the speed of the timers.
     * Default value is 0x03, which corresponds to a prescaler of 64.
     *
     * 0x01    / 0x02    / 0x03  / 0x04  / 0x05
     * 62.5kHz / 3.92kHz / 490Hz / 122Hz / 30.5Hz
     *
     * @param scale The prescaler value (default is 0x03).
     */
    void setPrescalerTimers(uint8_t scale = 0x03) {
        // ITS RECOMMENDED NOT TO USE TIMER0 ON ARDUINO TO ITS CONFLICT WITH MILLIS() AND DELAY() FUNCTION
        // TCCR0B = (TCCR0B & 0b11111000) | scale;
        TCCR1B = (TCCR1B & 0b11111000) | scale;
        // ITS RECOMMENDED NOT TO USE TIMER2 ON ARDUINO TO ITS CONFLICT WITH MILLIS() AND DELAY() FUNCTION
        // TCCR2B = (TCCR2B & 0b11111000) | scale;
        TCCR3B = (TCCR3B & 0b11111000) | scale;
        TCCR4B = (TCCR4B & 0b11111000) | scale;
        TCCR5B = (TCCR5B & 0b11111000) | scale;
    }


    String getDirectionAnimation(int dir, unsigned long interval) {
        static unsigned long lastAnimUpdate = 0;
        static int animFrame = 0;
        const char* ccwAnim[] = { "   ", ">  ", ">> ", ">>>" };
        const char* cwAnim[] = { "   ", "  <", " <<", "<<<" };
        const int animFrames = 4;

        unsigned long now = millis();
        if (now - lastAnimUpdate > interval) {
            lastAnimUpdate = now;
            animFrame = (animFrame + 1) % animFrames;
        }

        if (dir > 0) {
            return String(cwAnim[animFrame]);
        } else if (dir < 0) {
            return String(ccwAnim[animFrame]);
        } else {
            return " ";
        }
    }
}
