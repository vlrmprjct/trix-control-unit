#ifndef REED_CONTROL_H
#define REED_CONTROL_H

#include <Arduino.h>

int readReeds();

void pushReed(int buttonNr, void (*callback)());

void updateReedStates();

void setReedStates();

#endif
