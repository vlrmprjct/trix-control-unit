#include "config.h"
#include <Arduino.h>

int lastSpeed = 0;

/**
 * Controls the motor based on the encoder value.
 * DRV8871 Module
 *
 * @param encoderVal The value from the encoder, which determines the speed and direction.
 * @param enPin The pin connected to the motor driver enable pin.
 * @param in1Pin The pin connected to the motor driver input 1 pin.
 * @param in2Pin The pin connected to the motor driver input 2 pin.
 * @usage motorEncoderControl(encoderValue, MTR_MDL_1_ENA, MTR_MDL_1_IN1, MTR_MDL_1_IN2);
 */
void motorEncoderControl(int encoderVal, int enPin, int in1Pin, int in2Pin)
{
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

    //analogWrite(enPin, speed);
}

/**
 * Controls the motor based on the raw speed input.
 *
 * @param rawSpeed The raw speed value (0-1023) from the input device.
 * @param enPin The pin connected to the motor driver enable pin.
 * @param in1Pin The pin connected to the motor driver input 1 pin.
 * @param in2Pin The pin connected to the motor driver input 2 pin.
 * @usage motorControl(analogRead(MTR_MDL_1_CTRL), MTR_MDL_1_ENA, MTR_MDL_1_IN1, MTR_MDL_1_IN2);
 */
void motorControl(int rawSpeed, int enPin, int in1Pin, int in2Pin) {
    int speed;
    int percentSpeed;

    if (rawSpeed < 512) {
        speed = map(rawSpeed, 0, 511, -255, DEAD_RANGE * (-1)); // Rückwärtsbereich
        percentSpeed = map(abs(speed), DEAD_RANGE, 255, 0, 100); // Prozentsatz für Rückwärtsrichtung
    } else {
        speed = map(rawSpeed, 512, 1023, DEAD_RANGE, 255); // Vorwärtsbereich
        percentSpeed = map(speed, DEAD_RANGE, 255, 0, 100); // Prozentsatz für Vorwärtsrichtung
    }

    // Wenn die Geschwindigkeit zu gering ist, Motor ausschalten
    if (abs(speed) < (DEAD_RANGE + DEAD_THRESHOLD)) { // Motor ausschalten, wenn Geschwindigkeit zu gering
        digitalWrite(in1Pin, LOW);
        digitalWrite(in2Pin, LOW);
        analogWrite(enPin, 0); // Motor ausschalten
    } else if (speed < 0) {
        // Rückwärtsbetrieb
        digitalWrite(in1Pin, LOW);
        digitalWrite(in2Pin, HIGH);
        analogWrite(enPin, abs(speed)); // Geschwindigkeit setzen
    } else {
        // Vorwärtsbetrieb
        digitalWrite(in1Pin, HIGH);
        digitalWrite(in2Pin, LOW);
        analogWrite(enPin, speed); // Geschwindigkeit setzen
    }

    // Nur ausgeben, wenn sich die Geschwindigkeit geändert hat
    if (speed != lastSpeed) {
        // // Für Debugging (optional)
        // Serial.print("Raw: ");
        // Serial.print(rawSpeed);
        // Serial.print(" | Speed: ");
        // Serial.print(speed);
        // Serial.print(" | Percent: ");
        // Serial.print(percentSpeed); // Prozentuale Geschwindigkeit
        // Serial.println("%");

        lastSpeed = speed; // Aktuellen Speed speichern
    }
}

