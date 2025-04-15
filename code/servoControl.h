#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

#include <Adafruit_PWMServoDriver.h>

namespace ServoControl {
    constexpr int SERVOMIN = 140;
    constexpr int SERVOMAX = 715;
    constexpr int ANGLE_STRAIGHT  = 80;
    constexpr int ANGLE_DIVERGING = 100;

    void switchTurnout(Adafruit_PWMServoDriver& driver, int channel, bool diverging);
    void setAngle(Adafruit_PWMServoDriver& driver, int channel, int angle);
}

#endif
