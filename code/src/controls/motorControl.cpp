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

    void setValue(byte zone, int value) {
        value = constrain(value, 0, 255);

        // Determine chip and pot number from zone
        int csPin;
        byte potNum;

        if (zone == 0 || zone == 1) {
            // ZONE_A (0) or ZONE_B (1) -> Chip 1
            csPin = DIGIPOT_CS;
            potNum = zone; // 0 or 1
        } else {
            // ZONE_C (2) or ZONE_D (3) -> Chip 2
            csPin = DIGIPOT2_CS;
            potNum = zone - 2; // 0 or 1
        }

        byte command = (potNum & 0x01) << 4;
        digitalWrite(csPin, LOW);
        MotorControl::softSPIWrite(command);
        MotorControl::softSPIWrite((byte)value);
        digitalWrite(csPin, HIGH);
    }
}

