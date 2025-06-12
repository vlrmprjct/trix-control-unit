#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

namespace MotorControl {
    void setValue(int encoderVal, int in1Pin, int in2Pin);
    void smoothStop(volatile int& encoderValue, int step, unsigned long interval)
}


#endif
