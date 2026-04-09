#pragma once
#include "../../config.h"

namespace MotorControl {
    void softSPIWrite(byte dataOut);
    void setValue(byte potNum, int value);
    int rampUp(byte zone, int target, bool reset, unsigned long interval = 30);
    int rampDown(byte zone, int inputValue, bool trigger, bool cancel, int minValue = HBF_BRAKE_MIN, unsigned long brakeDelay = HBF_BRAKE_DELAY, unsigned long interval = 30);
}
