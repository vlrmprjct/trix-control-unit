#pragma once

// SERVO CALIBRATION OFFSETS (index = TurnoutControls enum value W1..W11)
//                                    W1   W2   W3   W4   W5   W6   W7   W8   W9  W10  W11
const int SERVO_OFFSET_STRAIGHT[]  = { 0, -10, -15, -20,  -3, -10,  -8, -10, -20,   0, -20 };
const int SERVO_OFFSET_DIVERGING[] = { 0, +10,   0, +15,  +6, +12, -10, +10,   0, +20,   0 };

