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
        "04 3D 36 6A 40 15 90",
        "BR132",
        62,
        82,
        76
    },
    {
        "04 08 3C 6A 40 15 91",
        "BR220",
        90,
        120,
        110
    }
};

inline const int profileCount = sizeof(profiles) / sizeof(UIDProfile);
