#pragma once

namespace MotorControl {
    int mapSpeedCurve(int raw);
    void softSPIWrite(byte dataOut);
    void setValue(byte potNum, int value);
    void rampValue(volatile int& value, int target = 80, int step = 2, unsigned long interval = 250);
    int rampDynamicValue(unsigned long elapsedTime, float spPosition, float epPosition, int initialValue, int endValue, float initialSpeed);
}
