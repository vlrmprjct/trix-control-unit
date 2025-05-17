#ifndef BUTTON_CONTROL_H
#define BUTTON_CONTROL_H

#include <Arduino.h>

int readButtons();

void pushButton(int buttonNr, void (*callback)());

void updateButtonStates();

void setButtonStates();

#endif
