#include "../core/config.h"
#include <Arduino.h>
#include <SPI.h>
#include <math.h>

static bool rampActive = false;
static int  rampStep   = 0;
static int  rampValue  = 0;
static const int RAMP_STEPS = 100;  // TOTAL ITERATIONS @ 25MS = 2.5S
static const int RAMP_START = 30;   // MINIMUM OUTPUT AT START OF RAMP

namespace MotorControl {

    void startRamp() {
        rampStep = 0;
        rampValue = 0;
        rampActive = true;
    }

    void stopRamp()  {
        rampActive = false;
        rampValue = 0;
        rampStep = 0;
    }

    bool isRampActive() {
        return rampActive;
    }

    int  getRampValue() {
        return rampValue;
    }

    // LOGARITHMIC TAPER RAMP: FAST INITIAL RISE, FLATTENS TOWARD TARGET
    // CURVE: output = target * sqrt(t) — LIKE AN AUDIO LOG-POT
    int rampUp(int target) {
        int targetAbs = abs(target);
        rampStep++;
        if (rampStep >= RAMP_STEPS) {
            rampActive = false;
            rampValue = 0; rampStep = 0;
            return targetAbs;
        }
        rampValue = targetAbs > 0
            ? max(RAMP_START, (int)(targetAbs * sqrt((float)rampStep / RAMP_STEPS)))
            : 0;
        return rampValue;
    }

    void setValue(byte zone, int value) {
        value = constrain(value, 0, 255);

        // Determine chip and pot number from zone
        int csPin;
        byte potNum;

        if (zone == 0 || zone == 1) {
            // ZONE_A (0) or ZONE_B (1) -> Chip 1
            csPin = DIGIPOT1_CS;
            potNum = zone; // 0 or 1
        } else {
            // ZONE_C (2) or ZONE_D (3) -> Chip 2
            csPin = DIGIPOT2_CS;
            potNum = zone - 2; // 0 or 1
        }

        byte command = (potNum & 0x01) << 4;

        SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
        digitalWrite(csPin, LOW);
        SPI.transfer(command);
        SPI.transfer((byte)value);
        digitalWrite(csPin, HIGH);
        SPI.endTransaction();
    }
}

