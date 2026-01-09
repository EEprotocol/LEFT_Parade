[Japanese docs](./README_ja.md)
# LEFT Parade Project

This is a so-called *left-hand device*.  
It is intended for situations where you use a pen display tablet or similar device with your right hand and want to trigger macros with your left hand.

## Specifications
- Connectivity is **Bluetooth only**. Wired connections are not supported.  
  This limitation is due to the ESP32-C3; however, by using a board such as the ESP32-S3 that supports USB HID, wired operation can be implemented.
- A battery can be installed (3.7 V Li-ion).  
  Connect it directly to the battery terminals on the surface of the ESP32-C3.
- Up to **80 different macros** can be configured using simultaneous key presses  
  (Arduino IDE is required to configure the macros).
- To enable the above behavior, commands are sent **when the key switch is released**, not when it is pressed.

## Circuit Diagram
![circuit scheme](./scheme.png "Circuit Scheme")

Pins **D0, D1, D5, and D6** are available as free pins.  
Feel free to use them for LEDs or other custom extensions.

## Bill of Materials
| Component | Price (JPY) |
|:-----------|------------:|
| PCB | 110 |
| Rotary encoder | 550 |
| Knob | 324 |
| ESP32-C3 | 1,080 |
| Keycap | 70 |
| Key socket | 160 |
| Key switch | 330 |
| Battery | 450 |
| (SN74HC139N) | 135 |
| DIP socket (8 × 2 rows) | 135 |
| **Total** | **3,344** |

## Macros
Work in progress.

## Todo
- [ ] Impliment the GAME mode which sends commnad when keys are pushed
- [ ] Fabricate a PCB
- [ ] Prepare the other version: RIGHT Parade


## Special Thanks
- [Mystfit/ESP32-BLE-CompositeHID](https://github.com/Mystfit/ESP32-BLE-CompositeHID)
