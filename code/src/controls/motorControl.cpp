#include "../../config.h"
#include <Arduino.h>

namespace MotorControl {

    /**
     * Controls the motor based on the encoder value.
     * DRV8871 Module
     *
     * @param encoderVal The value from the encoder, which determines the speed and direction.
     * @param in1Pin The pin connected to the motor driver input 1 pin.
     * @param in2Pin The pin connected to the motor driver input 2 pin.
     * @usage motorEncoderControl(encoderValue, MTR_MDL_1_IN1, MTR_MDL_1_IN2);
     */
    void setValue(int encoderVal, int in1Pin, int in2Pin) {
        int speed = abs(encoderVal);

        if (encoderVal > 5) {
            digitalWrite(in1Pin, LOW);
            analogWrite(in2Pin, speed);
        } else if (encoderVal < 5) {
            digitalWrite(in2Pin, LOW);
            analogWrite(in1Pin, speed);

        } else {
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, LOW);
        }
    }
}

