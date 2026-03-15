#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

#include <Adafruit_PWMServoDriver.h>

namespace ServoControl {
    constexpr int SERVOMIN = 170;
    constexpr int SERVOMAX = 700;
    constexpr int ANGLE_STRAIGHT  = 70;
    constexpr int ANGLE_DIVERGING = 120;

    void switchTurnout(Adafruit_PWMServoDriver& driver, int channel, bool diverging, int offset = 0);
    void setAngle(Adafruit_PWMServoDriver& driver, int channel, int angle);
}

#endif
