#include "config.h"
#include "motorControl.h"
#include "relayControl.h"

const int switchPin = 2; // Pin für den Schalter
int counter = 0; // Zähler für Schalterbetätigungen
int lastSwitchState = LOW; // Letzter Zustand des Schalters

void setup()
{
    Serial.begin(9600);

    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(switchPin, INPUT_PULLUP);

    pinMode(LATCH, OUTPUT);
    pinMode(CLOCK, OUTPUT);
    pinMode(DATA, OUTPUT);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
}

void loop()
{
    // int switchState = digitalRead(switchPin); // Zustand des Schalters lesen

    // // Wenn der Schalter geschlossen wird (von HIGH nach LOW wechselt)
    // if (switchState == LOW && lastSwitchState == HIGH) {
    //     counter++; // Zähler erhöhen
    //     Serial.print("Counter: ");
    //     Serial.println(counter); // Zählerstand ausgeben
    // }
    // lastSwitchState = switchState; // Aktuellen Zustand speichern


    // setRelay(2, true); // Relais 2 AN
    // delay(1000);

    // setRelay(1, true); // Relais 1 AN (Relais 2 bleibt AN!)
    // delay(1000);

    // setRelay(3, false); // Relais 3 AUS (Relais 1, 2, 4 bleiben wie sie sind)
    // delay(1000);

    // toggleRelay(5); // Relais 5 umschalten
    // delay(1000);

    // toggleRelay(24); // Relais 5 umschalten
    // delay(1000);

    motorControl(analogRead(MAIN_CTRL), ENA, IN1, IN2);
    delay(50);
}
