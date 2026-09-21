#define RGB_BUILTIN 38
#define RGB_BRIGHTNESS 50

void setup() {
}

void loop() {
  // Red - bright
  neopixelWrite(RGB_BUILTIN, 50, 0, 0);
  delay(300);

  // Green - medium brightness
  neopixelWrite(RGB_BUILTIN, 0, 25, 0);
  delay(600);

  // Blue - bright
  neopixelWrite(RGB_BUILTIN, 0, 0, 50);
  delay(300);

  // White - lower brightness
  neopixelWrite(RGB_BUILTIN, 15, 15, 15);
  delay(800);

  // Off
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
  delay(500);
}