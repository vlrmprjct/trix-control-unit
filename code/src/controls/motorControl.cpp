#include "../../config.h"
#include <Arduino.h>
#include <SPI.h>

namespace MotorControl {

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

        SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
        digitalWrite(csPin, LOW);
        SPI.transfer(command);
        SPI.transfer((byte)value);
        digitalWrite(csPin, HIGH);
        SPI.endTransaction();
    }
}

