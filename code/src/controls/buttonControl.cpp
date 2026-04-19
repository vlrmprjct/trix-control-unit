#include "../core/config.h"
#include <Arduino.h>

static long previousButtonStates = 0;
static long currentButtonStates = 0;

namespace ButtonControl {

    long readButtons() {
        long states = 0;

        digitalWrite(BTN_LATCH, LOW);
        delayMicroseconds(5);
        digitalWrite(BTN_LATCH, HIGH);

        for (int i = 0; i < BTN_COUNT; i++) {
            int bit = digitalRead(BTN_DATA);
            if (bit == HIGH) {
                states |= (1L << i);
            }
            digitalWrite(BTN_CLOCK, HIGH);
            delayMicroseconds(5);
            digitalWrite(BTN_CLOCK, LOW);
        }

        return states;
    }

    void pushButton(int buttonNr, void (*callback)()) {
        if (buttonNr < 1) return;
        long mask = (1L << (buttonNr - 1));

        bool wasPressed = previousButtonStates & mask;
        bool isPressed = currentButtonStates & mask;

        if (!wasPressed && isPressed) {
            callback();
        }
    }

    void updateStates() {
        currentButtonStates = readButtons();
    }

    void setStates(){
        previousButtonStates = currentButtonStates;
    }
}


