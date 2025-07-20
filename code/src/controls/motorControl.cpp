#include "../../config.h"
#include <Arduino.h>

namespace MotorControl {

    /**
     * @brief Maps the raw encoder value to a speed curve.
     * @param raw The raw encoder value (0-255).
     * @return Mapped speed value (0-255).
     * @usage int speed = mapSpeedCurve(rawValue);
     */
    int mapSpeedCurve(int raw) {
        float normalized = abs(raw) / 255.0;
        float curved = pow(normalized, 1.5);
        return (int)(curved * 255.0);
    }

    /**
     * @brief Controls the motor based on the encoder value for DRV8871 Module
     * @param encoderVal The value from the encoder, which determines the speed and direction.
     * @param in1Pin The pin connected to the motor driver input 1 pin.
     * @param in2Pin The pin connected to the motor driver input 2 pin.
     * @usage motorEncoderControl(encoderValue, MTR_MDL_1_IN1, MTR_MDL_1_IN2);
     */
    void setValue(int encoderVal, int in1Pin, int in2Pin) {
        int direction = encoderVal > 0 ? 1 : (encoderVal < 0 ? -1 : 0);
        int rawSpeed = abs(encoderVal);
        int speed = rawSpeed;

        if (direction > 0) {
            digitalWrite(in1Pin, LOW);
            analogWrite(in2Pin, speed);
        } else if (direction < 0) {
            digitalWrite(in2Pin, LOW);
            analogWrite(in1Pin, speed);
        } else {
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, LOW);
        }
    }

    /**
     * @brief Ramps the motor value up or down to a target value.
     * @param value The current value of the motor speed.
     * @param target The target value to ramp towards (default is 80).
     * @param step The increment or decrement step for each update (default is 2).
     * @param interval The time interval in milliseconds between updates (default is 250).
     */
    void rampValue(volatile int& value, int target, int step, unsigned long interval) {
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

    /**
     * @brief Calculates control value with dynamic delay (exponential).
     * @param elapsedTime Elapsed time since start (ms).
     * @param spPosition Position of the startpoint (cm).
     * @param epPosition Position of the endpoint (cm).
     * @param initialValue Start value (e.g. 100).
     * @param endValue Target value (e.g. 50).
     * @param initialSpeed Speed at initialValue (cm/s).
     * @return int Control value ( example 100 → 50).
     */
    int rampDynamicValue(unsigned long elapsedTime, float spPosition, float epPosition, int initialValue, int endValue, float initialSpeed) {
        float t = elapsedTime / 1000.0;
        float t_mp = spPosition / initialSpeed;

        // Phase 1: Before MP (constant speed)
        if (t <= t_mp) {
            float currentPosition = initialSpeed * t;
            return initialValue;
        }

        // Phase 2: Deceleration (exponential decay)
        float alpha = 0.08; // Deceleration constant (adjust)
        float x = initialValue * exp(-alpha * (t - t_mp));

        // Limit to endValue
        x = max(x, (float)endValue);

        // Calculate position (numerical integration)
        static float lastPosition = spPosition;
        static unsigned long lastTime = t_mp * 1000;
        float deltaT = (elapsedTime - lastTime) / 1000.0;
        lastPosition += (x * 0.1) * deltaT; // v(x) = k * x (here k=0.1)
        lastTime = elapsedTime;

        return (int)x;
    }
}
