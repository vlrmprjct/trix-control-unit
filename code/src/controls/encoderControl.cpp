#include "../../config.h"
#include "encoderControl.h"
#include <Arduino.h>

namespace EncoderControl {

    Encoder encoderZoneA   = {ENC_ZONE_A_CLK, ENC_ZONE_A_DT, ENC_ZONE_A_CLK_STATE, ENC_ZONE_A};
    Encoder encoderZoneB = {ENC_ZONE_B_CLK, ENC_ZONE_B_DT, ENC_ZONE_B_CLK_STATE, ENC_ZONE_B};

    void processEncoder(Encoder& enc) {
        int currentCLK = digitalRead(enc.clkPin);
        int currentDT  = digitalRead(enc.dtPin);

        if (currentCLK != enc.clkState) {
            int delta = (currentDT == currentCLK) ? 5 : -5;
            enc.raw = constrain(enc.raw + delta, -255, 255);
        }

        enc.clkState = currentCLK;

        int r = enc.raw;
        if (r == 0) {
            enc.value = 0;
            return;
        }

        int sign = (r > 0) ? 1 : -1;
        int absRaw = abs(r);

        int mapped = map(absRaw, 1, 255, ENC_MIN, ENC_MAX);
        enc.value = sign * mapped;
    }

    void processZoneA() {
        processEncoder(encoderZoneA);
    }

    void processZoneB() {
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
