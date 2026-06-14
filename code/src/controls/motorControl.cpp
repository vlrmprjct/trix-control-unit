#include "../core/config.h"
#include <Arduino.h>
#include <SPI.h>
#include <math.h>

static int hbfStart = 0;
static int hbfStep = 0;              // TIME COUNTER: 0 → RAMP_STEPS
static int hbfRamp = 0;              // COMPUTED OUTPUT VALUE (FOR DISPLAY + MOTOR)
static const int RAMP_STEPS = 100;   // TOTAL ITERATIONS @ 25MS = 2.5S
static int departingHBF = 0;         // 0=NONE, 1=HBF1, 2=HBF2 — SET WHILE TRAIN IS ON W2

namespace MotorControl {

    void setHbfStart(int val) {
        if (val == 1) {
            hbfStep = 0;
            hbfRamp = 0;
        }
        hbfStart = val;
    }

    int getHbfStart()            { return hbfStart; }
    int getHbfRamp()             { return hbfRamp; }
    void setDepartingHBF(int n)  { departingHBF = n; }
    int  getDepartingHBF()       { return departingHBF; }
    bool isW2InUse()             { return departingHBF != 0; }

    // LOGARITHMIC TAPER RAMP: FAST INITIAL RISE, FLATTENS TOWARD TARGET
    // CURVE: output = target * sqrt(t) — LIKE AN AUDIO LOG-POT
    int rampUp() {
        int encAabs = abs(ENC_ZONE_A);
        if (hbfStart == 1) {
            hbfStep++;
            if (hbfStep >= RAMP_STEPS) {
                hbfStart = 0;
                hbfRamp = encAabs;
                return encAabs;
            }
            hbfRamp = (int)(encAabs * sqrt((float)hbfStep / RAMP_STEPS));
            return hbfRamp;
        }
        hbfStep = 0;
        hbfRamp = 0;
        return encAabs;
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

