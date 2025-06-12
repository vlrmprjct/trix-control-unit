#include "../../config.h"
#include "servoControl.h"
#include <Arduino.h>

namespace ServoControl {

    void switchTurnout(Adafruit_PWMServoDriver& driver, int channel, bool diverging) {
        int angle = diverging ? ANGLE_DIVERGING : ANGLE_STRAIGHT;
        int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
        driver.setPWM(channel, 0, pulse);
    }

    void setAngle(Adafruit_PWMServoDriver& driver, int channel, int angle) {
        int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
        driver.setPWM(channel, 0, pulse);
    }

}
