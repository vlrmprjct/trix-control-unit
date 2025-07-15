#pragma once

namespace MotorControl {
    int mapSpeedCurve(int raw);
    void setValue(int encoderVal, int in1Pin, int in2Pin);
    void rampValue(volatile int& value, int target = 80, int step = 2, unsigned long interval = 250);
    int rampDynamicValue(unsigned long elapsedTime, float spPosition, float epPosition, int initialValue, int endValue, float initialSpeed);
}
