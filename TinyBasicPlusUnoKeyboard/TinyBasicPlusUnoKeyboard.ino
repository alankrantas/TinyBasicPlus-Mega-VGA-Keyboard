#define BAUDRATE 9600

#include <PS2KeyAdvanced.h>
#include <PS2KeyMap.h>

#define DATAPIN 2
#define IRQPIN 3

PS2KeyAdvanced keyboard;
PS2KeyMap keymap;

void setup() {
  Serial.begin(BAUDRATE);
  keyboard.begin(DATAPIN, IRQPIN);
  keyboard.setNoBreak(1);
  keyboard.setNoRepeat(1);
  keymap.selectMap((char *)"US");
}

void loop() {
  if (keyboard.available()) {
    uint16_t c = keymap.remapKey(keyboard.read()) & 0xFF;
    switch (c) {
      case 13:  // Enter
      case 32:  // Space
      case 33 ... 126:  // ascii characters
        Serial.write(c);
        break;
    }
  }
}
