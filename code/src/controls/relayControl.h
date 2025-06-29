#pragma once

namespace RelayControl {
    void initRelays();
    void setRelays(uint32_t value);
    void setRelay(int relayNumber, bool state);
    void toggleRelay(int relayNumber);
}
