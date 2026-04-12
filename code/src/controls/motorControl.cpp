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
}

