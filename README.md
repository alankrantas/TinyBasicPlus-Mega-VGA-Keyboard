# Tiny BASIC 8-bit Home Computer on Arduino with VGA and PS/2 Support

![IMG_3193](https://github.com/alankrantas/TinyBasicPlus-Mega-VGA-Keyboard/assets/44191076/fdf786c4-3ac2-4538-80a5-27d75ed2461a)

My version of remaking Rob Cai's [Arduino Basic PC With VGA Output](https://www.instructables.com/Arduino-Basic-PC-With-VGA-Output/), which is to use two Arduino AVR boards to create a 8-bit home computer running the [TinyBasic](https://en.wikipedia.org/wiki/Tiny_BASIC) runtime with VGA video output and PS/2 keyboard input.

Unlike Rob uses one Nano as the "VGA chip" and another as the TinyBasic runtime/keyboard controller, I run the VGA driver together with the TinyBasic on a Mega 2560 and use a Uno as the "keyboard adapter". I also tried to write a cleaner code and have the VGA driver to print characters according to their actual width (Rob set all of them with the same size of 5 VGA pixels).

The libraries I used includes:
* [TinyBasic Plus](https://github.com/BleuLlama/TinyBasicPlus) script by Scott Lawrence
* [VGAX](https://github.com/smaffer/vgax) driver by Sandro Maffiodo
* [PS2KeyAdvanced](https://github.com/techpaul/PS2KeyAdvanced) and [PS2KeyMap](https://github.com/techpaul/PS2KeyMap) drivers by Paul Carpenter

## Hardware

* An **Arduino Mega 2560** (script: ```TinyBasicPlusMegaVGA```)
* An **Arduino Uno** or **Arduino Nano** (script: ```TinyBasicPlusUnoKeyboard```)
* VGA and PS/2 connector breakout (optional)

There's many ways to connect VGA and PS/2 (see the driver repos for details) but personally I use a [Waveshare VGA PS2 Board](https://www.waveshare.com/vga-ps2-board.htm) which already have resistors on pins.

![VGA-PS2-Board_l](https://github.com/alankrantas/TinyBasicPlus-Mega-VGA-Keyboard/assets/44191076/c0193561-08cd-43c3-99f4-e07153ac8e93)

## Wiring

### Mega 2560

| Pin | VGA D-SUB 15 | Resistor |
| --- | --- | --- |
| GND | GND | |
| 9 | H-SYNC | 680Ω |
| 11 | V-SYNC | 680Ω |
| 30 | GREEN | 470Ω |
| 31 | BLUE | 470Ω |

![vga](https://github.com/alankrantas/TinyBasicPlus-Mega-VGA-Keyboard/assets/44191076/ee8b55ba-a909-41cc-8aea-ee2244344af0)

Note: The VGAX driver only uses two colors (you can change them to the combination you like, even connecting multiple colors to one pin).

### Uno

| Pin | PS/2 mini-DIN |
| --- | --- |
| 5V | VCC |
| GND | GND |
| 2 (data) | +DATA |
| 3 (irq) | +CLK |

![ps2-pinout-by-rones](https://github.com/alankrantas/TinyBasicPlus-Mega-VGA-Keyboard/assets/44191076/259b2eea-5ed6-40be-80d1-cef03efc363f)

### Between Mega and Uno

| Pin | Mega | Uno |
| --- | --- | --- |
| Serial port | 0 (Rx) | 1 (Tx) |
| GNE | GND | GND |

I also power the Uno directly using Mega's 5V pin (while the Mega can be powered by the USB cable) via the Waveshare VGA PS2 Board itself. So actually both the Uno and the PS/2 keyboard are powered from the Mega.

## Explanation for the Two-Board Combination

It is simply not possible to run both VGAX and PS/2 drivers together on the same board without rewrite them, due to both rely on timers for interrupt, dispite the MEGA and Uno are not using the same pins. On AVR boards ```timer0``` by default is used for built-in functions like ```delay``` and ```millis```, and VGAX uses ```timer0```, ```timer1``` and ```timer2```. Whereas VGAX does provide alternative functions like ```vga.millis()``` to avoid issues, the PS/2 drivers (Rob uses [PS2Keyboard](https://github.com/PaulStoffregen/PS2Keyboard)) still calls the original ```millis``` and thus break both driver timings.

### TinyBasic Memory Usage

By default the TinyBasic script use up all available memories on the board. I change it to use 6K so a little more than 4K is available when the TinyBlasic runtime is up and running.

I haven't test if other functionalities (buzzers, SD card, etc) will work. Later I'll try to remove checks for ESP boards since they are not necessary on MEGA.

### Supported Input Keys

I have the Uno's keyboard script and the VGAX printing function to only accept the following characters:

* Enter (ASCII code 13)
* Space (ASCII code 32)
* Characters of ASCII code 33~126

The TinyBasic script did support backspace, although it does not appear to work properly. Rob also implemented to use ESC to clear the screen, but I'll have to figure out how to reset the TinyBasic prompt at the same time.

### VGAX Colors

The VGAX, in order to save memory on Uno, only uses two colors so there are four colors available:

| Color value | Binary value | Effect |
| --- | --- | --- |
| ```0``` | ```00``` | Black |
| ```1``` | ```01``` | Color 1 (pin 30) |
| ```2``` | ```10``` | Color 2 (pin 31) |
| ```3``` | ```11``` | Both colors |

```vga.clear``` and ```vga.printPROGMEM``` alike use these color values despite they are in ```byte``` type.

The [VGAXUA](https://github.com/smaffer/vgaxua) displays only one color but use up significantly more memory due to the resolution is set higher.
