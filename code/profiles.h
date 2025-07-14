#pragma once

struct UIDProfile {
    const char* uid;
    const char* name;
    int min;
    int max;
    int brake;
};

inline UIDProfile profiles[] = {
    {
        "04 5F D9 98 C4 2A 81",
        "BR132",
        62,
        82,
        76
    },
    {
        "04 69 D9 98 C4 2A 81",
        "V200",
        74,
        120,
        110
    },
    {
        "04 63 D9 98 C4 2A 81",
        "BR220",
        80,
        115,
        105
    },
    {
        "04 61 D9 98 C4 2A 81",
        "BR228",
        54,
        90,
        80
    },
    {
        "04 6A D9 98 C4 2A 81",
        "BR211",
        62,
        74,
        65
    }
};

inline const int profileCount = sizeof(profiles) / sizeof(UIDProfile);
