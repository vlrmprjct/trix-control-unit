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

    int rampUp(byte zone, int target, bool reset, unsigned long interval) {
        static int currentOutput[2] = {0, 0};
        static unsigned long lastUpdate[2] = {0, 0};
        static bool active[2] = {false, false};

        if (reset) {
            currentOutput[zone] = 0;
            active[zone] = true;
            lastUpdate[zone] = millis();
        }

        // Not in soft-start phase: pass through directly
        if (!active[zone]) {
            return target;
        }

        unsigned long now = millis();
        if (now - lastUpdate[zone] >= interval) {
            lastUpdate[zone] = now;
            // Ease-in: larger steps when far from target, smaller steps when close
            int step = max(1, (target - currentOutput[zone]) / 12);
            currentOutput[zone] = min(currentOutput[zone] + step, target);
        }

        // Ramp complete: deactivate soft-start
        if (currentOutput[zone] >= target) {
            active[zone] = false;
        }

        return currentOutput[zone];
    }

    int rampDown(byte zone, int inputValue, bool trigger, bool cancel, int minValue, unsigned long brakeDelay, unsigned long interval) {
        static int currentOutput[2] = {0, 0};
        static unsigned long lastUpdate[2] = {0, 0};
        static unsigned long triggerTime[2] = {0, 0};
        static bool active[2] = {false, false};

        // Track input continuously when not active to avoid ISR timing issues at trigger moment
        if (!active[zone] && inputValue > 0) {
            currentOutput[zone] = inputValue;
        }

        if (cancel) {
            active[zone] = false;
            return inputValue;
        }

        if (trigger && !active[zone]) {
            active[zone] = true;
            triggerTime[zone] = millis();
            lastUpdate[zone] = millis();
        }

        if (!active[zone]) {
            return inputValue;
        }

        unsigned long now = millis();
        if (now - triggerTime[zone] >= brakeDelay) {
            if (now - lastUpdate[zone] >= interval) {
                lastUpdate[zone] = now;
                int step = max(1, (currentOutput[zone] - minValue) / 8);
                currentOutput[zone] = max(currentOutput[zone] - step, minValue);
            }
        }

        return currentOutput[zone];
    }
}

