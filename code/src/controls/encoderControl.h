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
        int raw;
    };

    extern Encoder encoderZoneA;
    extern Encoder encoderZoneB;

    void processEncoder(Encoder& enc);
    void processZoneA();
    void processZoneB();
    void syncDirections(Encoder& a, Encoder& b);

    Direction getDirection(const Encoder& enc);
}
