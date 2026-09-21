readme >ESP32-S3 Arduino RGB LED Lab
 
Lab 1: Getting Started with ESP32-S3 and Arduino IDE
Hardware and Software
Board: ESP32-S3-DevKitC-1 v1.1
RGB LED GPIO: GPIO 38
IDE: Arduino IDE 2.x
Board package: ESP32 by Espressif Systems
RGB control: neopixelWrite()
Serial baud rate: 115200
Serial Monitor Test
The ESP32-S3 was successfully connected to Arduino IDE and programmed using the ESP32S3 Dev Module board configuration.
The Serial Monitor was configured to 115200 baud. The test program successfully displayed serial messages from the ESP32-S3, confirming that the board was running correctly.
Task A - Change the Colour
I tested the onboard RGB LED using different RGB channel values.
The LED was successfully changed to:
Red: (50, 0, 0)
Green: (0, 50, 0)
Blue: (0, 0, 50)
White: (50, 50, 50)
Off: (0, 0, 0)
The same onboard RGB LED was used for all colours. The LED turned on for one second and then turned off for one second.
Observation
Changing the red, green, and blue channel values in neopixelWrite() changed the colour of the onboard RGB LED.
Task B - Change the Blink Rate
Task B: I changed both delay values from 1000 ms to 250 ms. The LED blinked much faster than in the original program. The original complete ON/OFF cycle was 2000 ms, while the modified cycle was 500 ms.
I changed both delay values from 1000 ms to 250 ms.
The original program had:
ON: 1000 ms
OFF: 1000 ms
Complete cycle: 2000 ms
The modified program had:
ON: 250 ms
OFF: 250 ms
Complete cycle: 500 ms
Observation
The LED blinked much faster after reducing the delays from 1000 ms to 250 ms.
Task C - RGB Cycle
Task C: I created a repeating RGB cycle using the onboard RGB LED. The sequence is red, green, blue, and off. Each state lasts 500 ms before changing to the next state. After the off state, the sequence repeats from red.
I created a repeating sequence:
Red → Green → Blue → Off
Each state lasted 500 ms.
Observation
The onboard RGB LED changed from red to green, then blue, then turned off. After the off state, the sequence repeated from red.
Task D - Personal LED Pattern
I created a custom pattern using different colours, brightness values, and delays.
The final pattern was:
Red → Green → Blue → White → Off
The RGB values and delays were:
State
Red
Green
Blue
Delay
Red
50
0
0
300 ms
Green
0
25
0
600 ms
Blue
0
0
50
300 ms
White
15
15
15
800 ms
Off
0
0
0
500 ms
Observation
The LED displayed each colour for a different amount of time. The green and white states also used lower brightness values. After the off state, the complete pattern repeated continuously.
Final Arduino Sketch
The final Arduino sketch is included in this repository as:
ESP32_RGB_Lab.ino
The sketch controls the onboard RGB LED on GPIO 38 using neopixelWrite().
Conclusion
The ESP32-S3 was successfully configured and programmed using Arduino IDE. Serial communication was tested at 115200 baud, and the onboard RGB LED was controlled using different colours, brightness levels, and timing delays. Tasks A, B, C, and D were completed and tested on the ESP32-S3.
***this readme made by AI 
