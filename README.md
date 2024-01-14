[![PlatformIO](https://github.com/hpsaturn/m5stickc-dcf77-hat/workflows/PlatformIO/badge.svg)](https://github.com/hpsaturn/m5stickc-dcf77-hat/actions/)

# M5StickC DCF77 HAT

Basic hat implementation of a DCF77 receiver on a M5StickC-Plus or compatible ESP32 devices. This firmware uses an [improvement of Arduino-DCF77 library](https://github.com/hpsaturn/Arduino-DCF77), using an event callback to improve the GUI and signal representation.

<table>
	<tr>
		<td>
			Don't forget to star ⭐ this repository
		</td>
	</tr>
</table>

## Device

The current firmware was tested on 77.5KHZ single frequency device. It is the 4 pins version and it was bought in [Aliexpress](https://www.aliexpress.com/item/1005005254051736.html).

## Wiring

| M5StickC pin | DCF77 pin | Description |
|:----:|:----:|:-------------|
| 0 | P | PON: Enable/Disable |
| 26 | T | Interrupt pin or data |
| 3.3v | VDD | Positive (3.3v) |
| GND  | GND | Ground / unlabeled pin |

(see the photos in [Thingiverse](https://www.thingiverse.com/thing:6033436))

## Usage

**Some tips:**  

- [x] Check the right device. Some devices has different frequencies for each country.
- [x] Try to leave the device from away any radio or wireless interference or noise. The radio is very susceptible to noise.
- [x] In indoors it works, but is possible that in some builds doesn't. Try in outdoors first.
- [x] The position of the device for me was important, in horizontal is possible that it works better

**Buttons:**

- Left button: Power ON/Off
- M5 Button: Backlight or brightness toggle
- Right button: Unimplementened  

## Installation

Please install first [PlatformIO](http://platformio.org/) open source ecosystem for IoT development compatible with **Arduino** IDE and its command line tools (Windows, MacOs and Linux). Also, you may need to install [git](http://git-scm.com/) in your system.

For the **default** board, the `M5StickCPlus`, only clone this and upload firmware via USB cable with a simple command:

`pio run --target upload`

or import the project in VSCode and build and run it.

## Box

Is a basic box - hat for these devices, for example M5StickC or M5StickC-Plus, also should be work in M5CoreInk device.

[![Box DCF77 M5StickCPlus Hat](box/collage.jpg)](https://youtu.be/C50qMhYiE60)

**Files:**  

![DCF77 box base](box/dfc77_holder_vertical.stl)  
![DCF77 box lid](box/dfc77_holder_vertical_lid.stl)  

Also in [Thingiverse](https://www.thingiverse.com/thing:6033436).  

---

## Thanks to

@felmue to fix a sporadical crash.  
@jcomas for the help and docs on https://t.me/esp_es  
The old [DCF77 library](https://github.com/thijse/Arduino-DCF77)  
The [Seiko documentation](https://www.seikowatches.com/instructions/html/SEIKO_8B92_EN/CWVVSYxxhbpilh)