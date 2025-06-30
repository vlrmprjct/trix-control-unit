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

        if (encoderVal > 1) {
            digitalWrite(in1Pin, LOW);
            analogWrite(in2Pin, speed);
        } else if (encoderVal < 1) {
            digitalWrite(in2Pin, LOW);
            analogWrite(in1Pin, speed);

        } else {
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, LOW);
        }
    }

    /**
     * Ramps the motor value up or down to a target value.
     * This function adjusts the motor speed gradually to avoid sudden changes.
     *
     * @param value The current value of the motor speed.
     * @param target The target value to ramp towards (default is 80).
     * @param step The increment or decrement step for each update (default is 2).
     * @param interval The time interval in milliseconds between updates (default is 250).
     */
    void rampValue(volatile int& value, int target = 80, int step = 2, unsigned long interval = 250) {
        static unsigned long lastUpdate = 0;
        unsigned long now = millis();

        if (now - lastUpdate >= interval) {
            lastUpdate = now;
            if (value < target) {
                value = min(value + step, target);
            } else if (value > target) {
                value = max(value - step, target);
            }
        }
    }

}
