#include "../../config.h"
#include "encoderControl.h"
#include <Arduino.h>

namespace EncoderControl {

    Encoder encoderZoneA   = {ENC_ZONE_A_CLK, ENC_ZONE_A_DT, ENC_ZONE_A_CLK_STATE, ENC_ZONE_A};
    Encoder encoderZoneB = {ENC_ZONE_B_CLK, ENC_ZONE_B_DT, ENC_ZONE_B_CLK_STATE, ENC_ZONE_B};

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
        processEncoder(encoderZoneA);
    }

    void processSecondary() {
        processEncoder(encoderZoneB);
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
