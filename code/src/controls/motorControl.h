#pragma once
#include "../core/config.h"

namespace MotorControl {
    void setValue(byte zone, int value);
    int  rampUp(int target);
    void startRamp();
    void stopRamp();
    bool isRampActive();
    int  getRampValue();
}
