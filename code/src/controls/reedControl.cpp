#include "../core/config.h"
#include <Arduino.h>

static long previousReedStates = 0;
static long currentReedStates = 0;

namespace ReedControl {

    long read() {
        long states = 0;

        digitalWrite(REED_LATCH, LOW);
        delayMicroseconds(5);
        digitalWrite(REED_LATCH, HIGH);

        for (int i = 0; i < 16; i++) {
            if (digitalRead(REED_DATA)  == HIGH) states |= (1L << i);
            if (digitalRead(REED2_DATA) == HIGH) states |= (1L << (i + 16));
            digitalWrite(REED_CLOCK, HIGH);
            delayMicroseconds(5);
            digitalWrite(REED_CLOCK, LOW);
        }
        // Serial.print("Reed states: ");
        // for (int i = 0; i < 16; i++) {
        //     // Serial.print((states >> i) & 1);
        //     if (i < 15) Serial.print(",");
        // }
        // Serial.println();
        // delayMicroseconds(500); // Ensure clock is stable before next read
        return states;
    }

    void push(int reedNr, void (*callback)()) {
        if (reedNr < 1) return;
        long mask = (1L << (reedNr - 1));

        bool wasPressed = previousReedStates & mask;
        bool isPressed = currentReedStates & mask;

        if (!wasPressed && isPressed) {
            callback();
        }
    }

    void updateStates() {
        currentReedStates = ReedControl::read();
    }

    void setStates() {
        previousReedStates = currentReedStates;
    }
}
