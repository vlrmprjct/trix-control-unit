#pragma once

struct HBF_SLOT {
    bool occupied;
    bool selected;
    bool powered;
    char name[32];
    char uid[32];
    int min;
    int max;
    int brake;
};

struct HBF_STATE {
    HBF_SLOT HBF1;
    HBF_SLOT HBF2;
    HBF_SLOT HBF3;
    HBF_SLOT HBF4;
    HBF_SLOT HBF5;
    HBF_SLOT HBF6;
    HBF_SLOT HBF7;
};

inline HBF_STATE HBF_ROUTE;
