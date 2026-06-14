# Relay Map

Relay module: 24-channel, controlled via shift register.
Index is 1-based. All relays default to OFF (false) on boot.

## Assignment

| Index | Name              | Controls                                    | ON = ...                        |
|-------|-------------------|---------------------------------------------|---------------------------------|
| 1     | BBF4_RELAY        | BBF4 — track power                          | power on                        |
| 2     | BBF1_RELAY        | BBF1 — track power                          | power on                        |
| 3     | BBF2_RELAY        | BBF2 — track power                          | power on                        |
| 4     | BBF3_RELAY        | BBF3 — track power                          | power on                        |
| 5     | HBF2_ZONE_RELAY   | HBF2 — parking/arrival zone power           | holding track powered           |
| 6     | HBF2_TRACK_RELAY  | HBF2 — departure track power (Zone A entry) | train released into Zone A      |
| 7     | HBF1_ZONE_RELAY   | HBF1 — parking/arrival zone power           | holding track powered           |
| 8     | HBF1_TRACK_RELAY  | HBF1 — departure track power (Zone A entry) | train released into Zone A      |
| 9     | BBF5_RELAY        | BBF5 — track power                          | power on                        |
| 10    | BLOCKB_RELAY      | Block B — entry power                        | entry open (train may enter)    |

## HBF Dual-Relay Logic

Each HBF station uses two relays:

- **ZONE_RELAY** (5 / 7): Powers the holding/parking section inside the station. Switched ON when the train arrives and parks, switched OFF on departure.
- **TRACK_RELAY** (6 / 8): Powers the departure track connecting the station to Zone A. Switched ON when the train departs (after digipot is pre-zeroed to avoid jerk).

Both relays are never ON simultaneously during a transition — `setValue(ZONE_A, 0)` is called before enabling `TRACK_RELAY` to prevent voltage spikes on the motor.

## Block B Relay (10)

Controls whether Block B (main loop entry) is powered.

- `ON (true)` — set when Zone B clears (RD_30 reed), or when an HBF train arrives and at least one station is free
- `OFF (false)` — set when a train enters Block B (RD_30), blocking further entry

## Unassigned

Relays 11–24 are currently unassigned.
