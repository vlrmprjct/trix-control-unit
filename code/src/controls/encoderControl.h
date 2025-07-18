#pragma once

namespace EncoderControl {

    enum Direction {
        STOP = 0,
        CW = 1,
        CCW = -1
    };

    struct Encoder {
        const int& clkPin;
        const int& dtPin;
        int& clkState;
        volatile int& value;
    };

    extern Encoder primaryEncoder;
    extern Encoder secondaryEncoder;

    void processEncoder(Encoder& enc);
    void processPrimary();
    void processSecondary();
    void syncDirections(Encoder& a, Encoder& b);

    Direction getDirection(const Encoder& enc);
}
