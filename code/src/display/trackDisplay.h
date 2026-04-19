#pragma once

#include "../core/state.h"

// LCD SLOT: TRACK REFERENCE + DISPLAY POSITION
struct LCDSlot {
    Tracks* track;
    const char* name;
    int colStart;
    int colEnd;
    int powerCol;
    int row;
};

// LCD LAYOUT TABLE (20x4)
// ROW 0: [>HBF1*] [>BBF1*] [>BBF4*]
// ROW 1: [>HBF2*] [>BBF2*] [>BBF5*]
// ROW 2:          [>BBF3*]
// ROW 3: [ENC_A]  [km/h]   [ENC_B]
const LCDSlot lcdSlots[] = {
    { &HBF1, "HBF1",  0,  4,  5, 0 },
    { &HBF2, "HBF2",  0,  4,  5, 1 },
    { &BBF1, "BBF1",  7, 11, 12, 0 },
    { &BBF2, "BBF2",  7, 11, 12, 1 },
    { &BBF3, "BBF3",  7, 11, 12, 2 },
    { &BBF4, "BBF4", 14, 18, 19, 0 },
    { &BBF5, "BBF5", 14, 18, 19, 1 },
};
