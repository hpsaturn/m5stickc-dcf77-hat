# M5StickC DCF77 HAT

Basic hat implementation of a DCF77 receiver for M5StickC-Plus or compatible ESP32 devices.

## Device

The current firmware was tested on 77.5KHZ single frequency device. It is the 4 pins version and it was bought here in [Aliexpress](https://www.aliexpress.com/item/1005005254051736.html).

## Wiring

| M5StickC pin | DCF77 pin | Description |
|:----:|:----:|:-------------|
| 0 | P | PON: Enable/Disable |
| 26 | T | Interrupt pin or data |
| 3.3v | VDD | Positive (3.3v) |
| GND  | GND | Ground / unlabeled pin |

## Box

Is a basic box - hat for these devices, for example M5StickC or M5StickC-Plus, also should be work in M5CoreInk device.

![DCF77 box base](box/dfc77_holder_vertical.stl)  
![DCF77 box lid](box/dfc77_holder_vertical_lid.stl)
