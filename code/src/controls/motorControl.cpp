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

    void softSPIWrite(byte dataOut) {
        for (int i = 7; i >= 0; i--) {
            // MOSI setzen
            digitalWrite(DIGIPOT_MOSI, (dataOut & (1 << i)) ? HIGH : LOW);
            // Clock-Puls erzeugen
            digitalWrite(DIGIPOT_SCK, HIGH); // steigende Flanke -> Bit wird übernommen
            digitalWrite(DIGIPOT_SCK, LOW); // zurücksetzen
        }
    }

    /**
     * @brief Sets the wiper position of the MCP4261 digital potentiometer.
     * @param potNum The potentiometer number (0 or 1).
     * @param value The wiper position value (0-255).
     * @usage setPot(0, 128); // Set potentiometer 0 to mid (128) position
     */
    void setValue(byte potNum, int value) {
        if (value < 0)
            value = 0;
        if (value > 255)
            value = 255;

        byte command = (potNum & 0x01) << 4; // 0x00 = Poti0, 0x10 = Poti1

        digitalWrite(DIGIPOT_CS, LOW);
        MotorControl::softSPIWrite(command);
        MotorControl::softSPIWrite((byte)value);
        digitalWrite(DIGIPOT_CS, HIGH);
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

    /**
     * @brief Applies a soft-start (ease-in) ramp to a motor zone output.
     * @param zone The motor zone index (e.g. ZONE_A = 0, ZONE_B = 1).
     * @param target The target output value (0-255), typically abs(ENC_ZONE_x).
     * @param reset When true, resets the internal output to 0 (use on powered rising edge).
     * @param interval Time in ms between each ramp step (default 30ms).
     * @return The current ramped output value to pass to setValue().
     */
    int applySoftStart(byte zone, int target, bool reset, unsigned long interval) {
        static int currentOutput[2] = {0, 0};
        static unsigned long lastUpdate[2] = {0, 0};
        static bool active[2] = {false, false};

        if (reset) {
            currentOutput[zone] = 0;
            active[zone] = true;
            lastUpdate[zone] = millis();
        }

        // Not in soft-start phase: pass through directly
        if (!active[zone]) {
            return target;
        }

        unsigned long now = millis();
        if (now - lastUpdate[zone] >= interval) {
            lastUpdate[zone] = now;
            // Ease-in: larger steps when far from target, smaller steps when close
            int step = max(1, (target - currentOutput[zone]) / 12);
            currentOutput[zone] = min(currentOutput[zone] + step, target);
        }

        // Ramp complete: deactivate soft-start
        if (currentOutput[zone] >= target) {
            active[zone] = false;
        }

        return currentOutput[zone];
    }
}
