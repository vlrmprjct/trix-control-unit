```
MAIN DIRECTION Counter clockwise

╡   Siding
╦   Turnout
■   Reed Switch
‡   Separation  Zone A / Zone B / ZONE C

                                            <---- MAIN DIRECTION

    ╔═════‡═══W7═╦═══W5═╦═■══════════════════════════ BBF1 ══════════════════■═╦═W4════╦═W3══ ZONE A ══╗
    ║            ║      ╚═■══════════════════════════ BBF2 ══════════════════■═╝       ║               ║
    ║            ╚═══W6═╦══■═════════════════════════ BBF3 ═══════════════════■═╦═W9═══╝               ║
    ║                   ╚══════W8═╦═■════════════════ BBF4 ═══════════■═╦═W10═══╝                      ■
    ║                             ╚═■════════════════ BBF5 ══════■══W11═╩══════════════════════╡       ║
    ║                                                                                                  ║
    ║        ╔═■══════════════W1═╦══■═══════════════ HBF1 ═════■══════════════════════════■════╦═W2══‡═╝
    ║        ║                   ╚══■═══════════════ HBF2 ═════■══════════════════════════■════╝
    ║        ║
    ║        ║
    ║        ╚══════════════════════■═══════════════════════════════ ZONE B ═══════════════════════════╗
    ║                                                                                                  ║
    ║                                                                                                  ║
    ║                                                                                                  ║
    ╚══■════════════════════════════════■═══════════════════════════ ZONE C ═══════════════════════‡═■═╝
                                            ----> MAIN DIRECTION
```

## Reed Switches — numbered in MAIN DIRECTION

The track reeds (`RD_xx`) are numbered in travel order (counter-clockwise). Station reeds
are named by their physical end (`_L` / `_R`, plus `_C` for HBF), not by number.

### Track reeds (in travel order)

| Reed    | Approx. position                         | Function (from code) |
|---------|------------------------------------------|----------------------|
| `RD_05` | ZONE A, before the BBF entry turnouts (W3) | pick a free BBF, set entry route (routing) |
| `RD_10` | leaving ZONE A → ZONE C (past W7 / the `‡`) | release ZONE A, claim ZONE C (`BLOCKC`), start speed measure |
| `RD_20` | ZONE C, 112 cm after `RD_10`             | end speed measure |
| `RD_30` | ZONE C exit → ZONE B                       | block ZONE B (`BLOCKB`), release ZONE C (`BLOCKC`), release a waiting BBF |
| `RD_40` | reserved (between `RD_30` and `RD_50`)   | *no handler — currently unused* |
| `RD_50` | ZONE B, before HBF                        | select target HBF (W1) |

### Station reeds (by physical end)

- **BBF** — `RD_BBFx_R` = entry (ZONE A side, sets `occupied`); `RD_BBFx_L` = exit (ZONE C side, `onBBFReedL`)
- **HBF** — `RD_HBFx_L` = entry (from ZONE B, sets `occupied`); `RD_HBFx_C` = middle (brake ramp); `RD_HBFx_R` = exit (near W2)

> Positions are inferred from the code comments + this schematic — correct any that differ.
