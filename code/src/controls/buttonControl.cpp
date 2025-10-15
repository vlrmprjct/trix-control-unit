#include "../../config.h"
#include <Arduino.h>

static int previousButtonStates = 0;
static int currentButtonStates = 0;

namespace ButtonControl {

    int readButtons() {
        int states = 0;

        digitalWrite(BTN_LATCH, LOW);
        delayMicroseconds(5);
        digitalWrite(BTN_LATCH, HIGH);

        for (int i = 0; i < BTN_COUNT; i++) {
            int bit = digitalRead(BTN_DATA);
            if (bit == HIGH) {
                states |= (1 << i);
            }
            digitalWrite(BTN_CLOCK, HIGH);
            delayMicroseconds(5);
            digitalWrite(BTN_CLOCK, LOW);
        }

        return states;
    }

    void pushButton(int buttonNr, void (*callback)()) {
        int mask = (1 << (buttonNr - 1));

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


