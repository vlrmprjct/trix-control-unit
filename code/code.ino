#include "config.h"
#include "motorControl.h"
#include "relayControl.h"

const int switchPin = 2; // Pin für den Schalter
int counter = 0; // Zähler für Schalterbetätigungen
int lastSwitchState = LOW; // Letzter Zustand des Schalters

void setup() {
    Serial.begin(9600);

    pinMode(switchPin, INPUT_PULLUP);

    pinMode(MTR_MDL_1_ENA, OUTPUT);
    pinMode(MTR_MDL_1_IN1, OUTPUT);
    pinMode(MTR_MDL_1_IN2, OUTPUT);

    pinMode(MTR_MDL_2_ENB, OUTPUT);
    pinMode(MTR_MDL_2_IN3, OUTPUT);
    pinMode(MTR_MDL_2_IN4, OUTPUT);

    pinMode(RELAY_LATCH, OUTPUT);
    pinMode(RELAY_CLOCK, OUTPUT);
    pinMode(RELAY_DATA, OUTPUT);

    pinMode(MTR_MDL_1_CTRL, INPUT);
    pinMode(MTR_MDL_2_CTRL, INPUT);

    digitalWrite(MTR_MDL_1_IN1, LOW);
    digitalWrite(MTR_MDL_1_IN2, LOW);

    for (int i = 0; i < RELAY_COUNT; i++) {
        setRelay(i, false);
    }
}

void loop() {
    // int switchState = digitalRead(switchPin); // Zustand des Schalters lesen

    // // Wenn der Schalter geschlossen wird (von HIGH nach LOW wechselt)
    // if (switchState == LOW && lastSwitchState == HIGH) {
    //     counter++; // Zähler erhöhen
    //     Serial.print("Counter: ");
    //     Serial.println(counter); // Zählerstand ausgeben
    // }
    // lastSwitchState = switchState; // Aktuellen Zustand speichern

    static unsigned long lastToggleTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastToggleTime >= 5000) {
        toggleRelay(5);
        toggleRelay(1);
        toggleRelay(24);
        setRelay(17, true);
        lastToggleTime = currentTime;
    }

    setRelay(18, true);

    motorControl(analogRead(MTR_MDL_1_CTRL), MTR_MDL_1_ENA, MTR_MDL_1_IN1, MTR_MDL_1_IN2);
    delay(50);
}
