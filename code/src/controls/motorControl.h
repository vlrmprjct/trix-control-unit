#pragma once
#include "../core/config.h"

namespace MotorControl {
    void setValue(byte zone, int value);
    int rampUp();
    void setHbfStart(int val);
    int getHbfStart();
    int getHbfRamp();
    void setDepartingHBF(int n);
    int  getDepartingHBF();
    bool isW2InUse();
}
