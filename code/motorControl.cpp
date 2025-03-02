#include "config.h"
#include <Arduino.h>

int lastSpeed = 0;

void motorControl(int rawSpeed, int enPin, int in1Pin, int in2Pin)
{
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
        // Für Debugging (optional)
        Serial.print("Raw: ");
        Serial.print(rawSpeed);
        Serial.print(" | Speed: ");
        Serial.print(speed);
        Serial.print(" | Percent: ");
        Serial.print(percentSpeed); // Prozentuale Geschwindigkeit
        Serial.println("%");

        lastSpeed = speed; // Aktuellen Speed speichern
    }
}
