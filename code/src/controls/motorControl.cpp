#include "../core/config.h"
#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#include <math.h>

static bool rampActive = false;
static int  rampStep   = 0;
static int  rampValue  = 0;
static const int RAMP_STEPS = 100;  // TOTAL ITERATIONS @ 25MS = 2.5S
static const int RAMP_START = 30;   // MINIMUM OUTPUT AT START OF RAMP

static bool rampDownActive = false;
static int  rampDownStep   = 0;
static int  rampDownValue  = 0;
static const int RAMP_DOWN_STEPS = 120;  // TOTAL ITERATIONS @ 25MS = 3.0S
static const int RAMP_DOWN_MIN   = 42;   // MINIMUM OUTPUT NEVER STALL
static const float RAMP_DOWN_EXP = 1.5f; // CURVE EXPONENT: 1=LINEAR, 2=GENTLE, 3=STEEP

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

    void startRampDown() {
        rampDownStep = 0;
        rampDownValue = 0;
        rampDownActive = true;
    }

    void stopRampDown()  {
        rampDownActive = false;
        rampDownValue = 0;
        rampDownStep = 0;
    }

    bool isRampDownActive() {
        return rampDownActive;
    }

    int  getRampDownValue() {
        return rampDownValue;
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

    // CONFIGURABLE EASE-IN BRAKE RAMP: SLOW INITIAL DROP, ACCELERATES TOWARD END
    // CURVE: output = target * (1 - t^RAMP_DOWN_EXP) — NEVER DROPS BELOW RAMP_DOWN_MIN
    int rampDown(int target) {
        int targetAbs = abs(target);
        rampDownStep++;
        float t = min(1.0f, (float)rampDownStep / RAMP_DOWN_STEPS);
        rampDownValue = max(RAMP_DOWN_MIN, (int)(targetAbs * (1.0f - pow(t, RAMP_DOWN_EXP))));
        return rampDownValue;
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

    // ONE-TIME NV-WIPER PROVISIONING (EEPROM-GUARDED) ###########################################
    static const uint8_t NV_ZEROED_MARK = 0xA5;

    // WRITE ONE NON-VOLATILE WIPER REGISTER TO ZERO-SCALE (nvAddr: 0x2 = NV0, 0x3 = NV1)
    static void writeNVWiper(int csPin, byte nvAddr) {
        SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
        digitalWrite(csPin, LOW);
        SPI.transfer((nvAddr << 4) | 0x00);   // ADDR + WRITE-CMD(00) + D9:D8 = 0
        SPI.transfer(0x00);                    // D7:D0 = 0 -> ZERO-SCALE
        digitalWrite(csPin, HIGH);
        SPI.endTransaction();
        delay(10);                             // NV EEPROM WRITE CYCLE (~5-10ms), Vcc MUST STAY STABLE
    }

    // RUNS EXACTLY ONCE: PERSIST NV WIPERS = 0 SO THE MCP4261 POWERS UP AT 0V (NO COLD-BOOT ~6V).
    // GUARDED BY AN EEPROM MARKER — WRITES THE CHIP ONCE, THEN RETURNS IMMEDIATELY ON EVERY BOOT.
    void ensureNVWiperZeroed() {
        if (EEPROM.read(EEPROM_DIGIPOT) == NV_ZEROED_MARK) return;   // ALREADY DONE
        writeNVWiper(DIGIPOT1_CS, 0x2);  writeNVWiper(DIGIPOT1_CS, 0x3);   // CHIP 1: ZONE_A / ZONE_B
        writeNVWiper(DIGIPOT2_CS, 0x2);  writeNVWiper(DIGIPOT2_CS, 0x3);   // CHIP 2: ZONE_C / ZONE_D
        EEPROM.write(EEPROM_DIGIPOT, NV_ZEROED_MARK);
    }
}

