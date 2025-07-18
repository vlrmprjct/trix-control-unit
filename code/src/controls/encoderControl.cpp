#include "../../config.h"
#include "encoderControl.h"
#include <Arduino.h>

namespace EncoderControl {

    Encoder primaryEncoder   = {ENC_PRIMARY_CLOCK, ENC_PRIMARY_DT, ENC_PRIMARY_CLOCK_STATE, ENC_PRIMARY_VALUE};
    Encoder secondaryEncoder = {ENC_SECONDARY_CLOCK, ENC_SECONDARY_DT, ENC_SECONDARY_CLOCK_STATE, ENC_SECONDARY_VALUE};

    void processEncoder(Encoder& enc) {
        int currentCLKState = digitalRead(enc.clkPin);
        int currentDTState = digitalRead(enc.dtPin);

        if (currentCLKState != enc.clkState) {
            int delta = (currentDTState == currentCLKState) ? 1 : -1;
            enc.value += delta;
        }

        enc.value = constrain(enc.value, -255, 255);
        enc.clkState = currentCLKState;
    }

    void processPrimary() {
        processEncoder(primaryEncoder);
    }

    void processSecondary() {
        processEncoder(secondaryEncoder);
    }

    void syncDirections(Encoder& a, Encoder& b) {
        int dirA = (a.value > 0) - (a.value < 0);
        int dirB = (b.value > 0) - (b.value < 0);

        static int lastDirA = 0;
        static int lastDirB = 0;

        if (dirA == dirB || dirA == 0 || dirB == 0) {
            lastDirA = dirA;
            lastDirB = dirB;
            return;
        }

        if (dirA != lastDirA) {
            b.value *= -1;
            lastDirB = -dirB;
            lastDirA = dirA;
        } else if (dirB != lastDirB) {
            a.value *= -1;
            lastDirA = -dirA;
            lastDirB = dirB;
        }
    }


    Direction getDirection(const Encoder& enc) {
        if (enc.value > 0) return CW;
        if (enc.value < 0) return CCW;
        return STOP;
    }
}
