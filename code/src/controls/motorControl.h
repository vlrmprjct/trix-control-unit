#pragma once

namespace MotorControl {
    void setValue(int encoderVal, int in1Pin, int in2Pin);
    void rampValue(volatile int& value, int target, int step, unsigned long interval);
}
