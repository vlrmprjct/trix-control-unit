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

    /**
     * Smoothly stops the motor by gradually reducing the encoder value.
     * This function is useful for preventing abrupt stops and ensuring a smoother operation.
     *
     * @param value The current value of the encoder, which will be modified to stop the motor.
     * @param step The amount by which to reduce the encoder value each time this function is called.
     * @param interval The time interval in milliseconds between successive calls to this function.
     */
    void smoothStop(volatile int& value, int step = 2, unsigned long interval = 70) {
        static unsigned long lastUpdate = 0;
        unsigned long now = millis();

        if (now - lastUpdate >= interval) {
            lastUpdate = now;
            if (value > 50) {
                value = max(50, value - step);
            } else if (value < -50) {
                value = min(-50, value + step);
            }
        }
    }
}

