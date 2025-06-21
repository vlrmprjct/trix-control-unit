#pragma once

struct HBF_STATE {
    bool HBF1;
    bool HBF2;
    bool HBF3;
};

inline HBF_STATE HBF_ROUTE;
inline HBF_STATE HBF_ACTIVE;

struct BBF_STATE {
    bool BBF1;
    bool BBF2;
    bool BBF3;
    bool BBF4;
    bool BBF5;
    bool BBF6;
};

inline BBF_STATE BBF_ROUTE;
inline BBF_STATE BBF_ACTIVE;
