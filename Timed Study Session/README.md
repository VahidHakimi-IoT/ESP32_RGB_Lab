# ESP32-S3 Study Session Timer

## Project Overview

This project implements a 10-second study session timer using an ESP32-S3 development board, a 0.96-inch SSD1306 OLED display, and the board's onboard RGB LED.

The user starts a session by sending the character `s` through the Arduino IDE Serial Monitor. During the session, the OLED displays the remaining time. The RGB LED indicates that the session is active and turns off when the session is complete.

## Hardware

- ESP32-S3 development board with ESP32-S3-WROOM-1 module
- 0.96-inch SSD1306 OLED display (128 × 64 pixels)
- Onboard addressable RGB LED
- USB cable
- Breadboard and jumper wires

## Wiring

| OLED pin | ESP32-S3 pin |
|---|---|
| GND | GND |
| VCC | 3.3V |
| SDA | GPIO8 |
| SCL | GPIO9 |

The OLED uses I²C communication at address `0x3C`.

The onboard RGB LED uses GPIO38. No external LED is required.

## Software and Libraries

- Arduino IDE
- ESP32 board support package
- Adafruit GFX Library
- Adafruit SSD1306 Library
- Adafruit NeoPixel Library
- FreeRTOS functionality provided by the ESP32 Arduino framework

## Arduino IDE Configuration

- Board: ESP32-S3 (select the appropriate installed board profile)
- Serial Monitor baud rate: 115200
- OLED I²C address: `0x3C`
- RGB LED GPIO: 38

## How the Program Works

The application uses two FreeRTOS software timers:

1. **Update timer:** Auto-reload timer with a 1000 ms period. It sends an update notification to the application task every second.
2. **Completion timer:** One-shot timer with a 10000 ms period. It sends a completion notification when the session ends.

The application task handles Serial Monitor input, OLED updates, and RGB LED control.

The timer callbacks only notify the application task. They do not directly update the OLED or control the LED.

When the user sends `s`, the application starts a session, records its start time, turns on the RGB LED, and starts both timers. Additional `s` commands are ignored while a session is active.

The remaining time is calculated using elapsed time from `millis()`. When both update and completion notifications are pending, completion is handled first.

## Test Results

| Test | Observed result | Status |
|---|---|---|
| Main sketch compilation and upload | Uploaded successfully | PASS |
| OLED initialization and countdown | OLED countdown works | PASS |
| RGB LED operation | LED works using GPIO38 | PASS |
| Serial Monitor communication | Start and completion messages work | PASS |
| Extra start command during a session | Not yet separately confirmed | PENDING |
| Exact 10-second completion timing | Not yet separately measured | PENDING |

## Conclusion

The ESP32-S3 study session timer was successfully uploaded and tested. The OLED countdown, onboard RGB LED, and Serial Monitor communication all work as expected.

The project demonstrates the use of FreeRTOS software timers, task notifications, I²C display communication, and addressable RGB LED control.

## Assignment Questions

Add the five answers required by the assignment here.
