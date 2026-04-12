#include "../../config.h"
#include <Arduino.h>

namespace MotorControl {

    void softSPIWrite(byte dataOut) {
        for (int i = 7; i >= 0; i--) {
            digitalWrite(DIGIPOT_MOSI, (dataOut & (1 << i)) ? HIGH : LOW);
            digitalWrite(DIGIPOT_SCK, HIGH);
            digitalWrite(DIGIPOT_SCK, LOW);
        }
    }

    void setValue(byte potNum, int value) {
        value = constrain(value, 0, 255);
        byte command = (potNum & 0x01) << 4;
        digitalWrite(DIGIPOT_CS, LOW);
        MotorControl::softSPIWrite(command);
        MotorControl::softSPIWrite((byte)value);
        digitalWrite(DIGIPOT_CS, HIGH);
    }

    static bool rampUpPending[2]   = {false, false};
    static bool rampDownPending[2] = {false, false};
    static bool rampDownCancel[2]  = {false, false};

    void triggerRampUp(byte zone)   { rampUpPending[zone]   = true; }
    void triggerRampDown(byte zone) { rampDownPending[zone] = true; }
    void cancelRampDown(byte zone)  { rampDownCancel[zone]  = true; }

    int rampUp(byte zone, int target, unsigned long startDelay, unsigned long interval) {
        static int currentOutput[2]       = {0, 0};
        static unsigned long lastUpdate[2] = {0, 0};
        static unsigned long triggeredAt[2]= {0, 0};
        static bool active[2]             = {false, false};

        if (rampUpPending[zone]) {
            rampUpPending[zone]  = false;
            currentOutput[zone]  = 0;
            active[zone]         = true;
            triggeredAt[zone]    = millis();
            lastUpdate[zone]     = millis();
        }

        if (!active[zone]) return target;

        // hold at 0 until startDelay has passed
        if (millis() - triggeredAt[zone] < startDelay) return 0;

        unsigned long now = millis();
        if (now - lastUpdate[zone] >= interval) {
            lastUpdate[zone] = now;
            int step = max(1, (target - currentOutput[zone]) / 12);
            currentOutput[zone] = min(currentOutput[zone] + step, target);
        }

        if (currentOutput[zone] >= target) active[zone] = false;

        return currentOutput[zone];
    }

    int rampDown(byte zone, int inputValue, int minValue, unsigned long brakeDelay, unsigned long interval) {
        static int currentOutput[2]         = {0, 0};
        static unsigned long lastUpdate[2]   = {0, 0};
        static unsigned long triggerTime[2]  = {0, 0};
        static bool active[2]                = {false, false};
        static bool holding[2]               = {false, false};

        if (!active[zone] && !holding[zone] && inputValue > 0) {
            currentOutput[zone] = inputValue;
        }

        if (rampDownCancel[zone]) {
            rampDownCancel[zone]  = false;
            rampDownPending[zone] = false;
            active[zone]          = false;
            holding[zone]         = false;
            return inputValue;
        }

        if (holding[zone]) return currentOutput[zone];

        if (rampDownPending[zone] && !active[zone]) {
            rampDownPending[zone] = false;
            active[zone]          = true;
            triggerTime[zone]     = millis();
            lastUpdate[zone]      = millis();
        }

        if (!active[zone]) return inputValue;

        unsigned long now = millis();
        if (now - triggerTime[zone] >= brakeDelay) {
            if (now - lastUpdate[zone] >= interval) {
                lastUpdate[zone] = now;
                int step = max(1, (currentOutput[zone] - minValue) / 8);
                currentOutput[zone] = max(currentOutput[zone] - step, minValue);
            }
        }

        if (currentOutput[zone] <= minValue) {
            active[zone]  = false;
            holding[zone] = true;
        }

        return currentOutput[zone];
    }
}

