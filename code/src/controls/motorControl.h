#pragma once
#include "../core/config.h"

namespace MotorControl {
    void setValue(byte zone, int value);
    // ZONE A: HBF DEPARTURE SOFTSTART
    int  rampUp(int target);
    void startRamp();
    void stopRamp();
    bool isRampActive();
    int  getRampValue();
    // ZONE B: HBF ARRIVAL BRAKE
    int  rampDown(int target);
    void startRampDown();
    void stopRampDown();
    bool isRampDownActive();
    int  getRampDownValue();
    // ONE-TIME (EEPROM-GUARDED): PERSIST MCP4261 NV WIPERS = 0 SO IT POWERS UP AT 0V
    void ensureNVWiperZeroed();
}
