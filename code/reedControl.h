#ifndef REED_CONTROL_H
#define REED_CONTROL_H

#include <Arduino.h>

namespace ReedControl {
    int read();
    void push(int buttonNr, void (*callback)());
    void updateStates();
    void setStates();
}

#endif
