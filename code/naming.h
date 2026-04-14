#pragma once

enum MotorControls {
    ZONE_A = 0,
    ZONE_B = 1,
    ZONE_C = 2,
    ZONE_D = 3,
};

enum HBFControls {
    SW_HBF1 =  1,
    SW_HBF2 =  2,
    BTN_HBF1 = 8,
    BTN_HBF2 = 7,
};

enum BBFControls {
    SW_BBF1 =   9,
    SW_BBF2 =  10,
    SW_BBF3 =  11,
    BTN_BBF1 = 16,
    BTN_BBF2 = 15,
    BTN_BBF3 = 14,
    SW_BBF4  = 17,
    SW_BBF5  = 18,
    BTN_BBF4 = 24,
    BTN_BBF5 = 23,
};

enum CommonControls {
    BTN_BLOCKA_OVERRIDE = 6,
};

enum TurnoutControls {
    W1 =   0,
    W2 =   1,
    W3 =   2,
    W4 =   3,
    W5 =   4,
    W6 =   5,
    W7 =   6,
    W8 =   7,
    W9 =   8,
    W10 =  9,
    W11 = 10,
};

enum ReedControls {
    RD_HBF1_L  =  8,
    RD_HBF1_C  =  7,
    RD_HBF1_R  =  6,
    RD_HBF2_L  =  5,
    RD_HBF2_C  =  4,
    RD_HBF2_R  =  3,
    RD_BBF1_R  =  2,
    RD_BBF1_L  =  1, // ?? KEINE REAKTION, KABEL ?
    RD_BBF2_R  = 16,
    RD_BBF2_L  = 15,
    RD_BBF3_R  = 14,
    RD_BBF3_L  = 13,
    RD_BBF4_R  = 12,
    RD_BBF4_L  = 11,
    RD_BBF5_R  = 10,
    RD_BBF5_L  =  9,
    RD_10      = 29, // !! NC
    RD_20      = 26,
    RD_30      = 25,
    RD_40      = 27,
    RD_50      = 28,
};

