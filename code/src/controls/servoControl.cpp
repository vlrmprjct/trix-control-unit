#include "../../config.h"
#include "servoControl.h"
#include <Arduino.h>

namespace ServoControl {

    void switchTurnout(Adafruit_PWMServoDriver& driver, int channel, bool diverging, int offset) {
        int angle = (diverging ? ANGLE_DIVERGING : ANGLE_STRAIGHT) + offset;
        angle = constrain(angle, 0, 180);
        int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
        driver.setPWM(channel, 0, pulse);
    }

    void setAngle(Adafruit_PWMServoDriver& driver, int channel, int angle) {
        angle = constrain(angle, 0, 180);
        int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
        driver.setPWM(channel, 0, pulse);
    }

}
