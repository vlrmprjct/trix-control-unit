#pragma once
#include "../../config.h"

namespace MotorControl {
    void softSPIWrite(byte dataOut);
    void setValue(byte potNum, int value);
    void triggerRampUp(byte zone);
    void triggerRampDown(byte zone);
    void cancelRampDown(byte zone);
    int rampUp(byte zone, int target, unsigned long startDelay = HBF_START_DELAY, unsigned long interval = 30);
    int rampDown(byte zone, int inputValue, int minValue = HBF_BRAKE_MIN, unsigned long brakeDelay = HBF_BRAKE_DELAY, unsigned long interval = 30);
}
