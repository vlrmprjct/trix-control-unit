#include "../../config.h"
#include <Arduino.h>

static int previousReedStates = 0;
static int currentReedStates = 0;

namespace ReedControl {

    int read() {
        int states = 0;

        digitalWrite(REED_LATCH, LOW);
        delayMicroseconds(5);
        digitalWrite(REED_LATCH, HIGH);

        for (int i = 0; i < 8; i++) {
            int bit = digitalRead(REED_DATA);
            if (bit == HIGH) {
                states |= (1 << i);
            }
            digitalWrite(REED_CLOCK, HIGH);
            delayMicroseconds(5);
            digitalWrite(REED_CLOCK, LOW);
        }

        return states;
    }

    void push(int reedNr, void (*callback)()) {
        int mask = (1 << (reedNr - 1));

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
