#ifndef RELAYCONTROL_H
#define RELAYCONTROL_H

void setRelays(uint32_t value);
void setRelay(int relayNumber, bool state);
void toggleRelay(int relayNumber);

#endif
