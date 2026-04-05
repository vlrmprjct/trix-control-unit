# Button Shift Register – Pin Mapping
# 3x 74HC165, daisy-chained → BTN_COUNT = 24

Arduino ←── [IC1: Pins 1-8] ←── [IC2: Pins 9-16] ←── [IC3: Pins 17-24]
  DATA

## IC1 – Buttons 1–8

```
                    ┌────────┐
         SH/LD  1 ──┤        ├── 16  VCC
           CLK  2 ──┤        ├── 15  CLK INH GND
         – [E]  3 ──┤        ├── 11  [A] – SW_HBF1
         – [F]  4 ──┤  IC1   ├── 12  [B] – SW_HBF2
BTN_HBF2 – [G]  5 ──┤        ├── 13  [C] –
BTN_HBF1 – [H]  6 ──┤        ├── 14  [D] –
           ~QH  7 ──┤        ├── 10  SER (← QH IC2)
           GND  8 ──┤        ├──  9  QH  (→ Arduino DATA)
                    └────────┘
```

## IC2 – Buttons 9–16

```
                    ┌────────┐
         SH/LD  1 ──┤        ├── 16  VCC
           CLK  2 ──┤        ├── 15  CLK INH GND
    free – [E]  3 ──┤        ├── 11  [A] – SW_BBF1
BTN_BBF3 – [F]  4 ──┤  IC2   ├── 12  [B] – SW_BBF2
BTN_BBF2 – [G]  5 ──┤        ├── 13  [C] – SW_BBF3
BTN_BBF1 – [H]  6 ──┤        ├── 14  [D] –
           ~QH  7 ──┤        ├── 10  SER (← QH IC3)
           GND  8 ──┤        ├──  9  QH  (→ SER IC1)
                    └────────┘
```

## IC3 – Buttons 17–24

```
                    ┌────────┐
         SH/LD  1 ──┤        ├── 16  VCC
           CLK  2 ──┤        ├── 15  CLK INH GND
         – [E]  3 ──┤        ├── 11  [A] – SW_BBF4
         – [F]  4 ──┤  IC3   ├── 12  [B] – SW_BBF5
BTN_BBF5 – [G]  5 ──┤        ├── 13  [C] –
BTN_BBF4 – [H]  6 ──┤        ├── 14  [D] –
           ~QH  7 ──┤        ├── 10  SER (← GND)
           GND  8 ──┤        ├──  9  QH  (→ SER IC2)
                    └────────┘
```
