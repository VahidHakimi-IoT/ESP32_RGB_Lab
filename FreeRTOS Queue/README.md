# ESP32 FreeRTOS Queue Assignment

## Purpose

In this assignment, I worked with FreeRTOS on an ESP32 to understand how tasks can communicate with each other using a queue.

## How It Works

I created two tasks: a Sensor Task and a Display Task.

- The Sensor Task starts with a temperature value of 20.
- Every second, it increases the value by 1 and sends it to the queue.
- The queue can hold up to 5 integer values.
- The Display Task receives the values from the queue and prints them in the Serial Monitor.

The temperature is simulated in the code, so it does not come from a real sensor.

## Serial Monitor

When I uploaded the code to the ESP32, I could see the temperature values appearing in the Serial Monitor. The values started at 21°C and increased by 1 every second.



## Reflection

This assignment helped me understand how two FreeRTOS tasks can work separately and still share information. I learned that a queue is useful because it provides a way for one task to send data and another task to receive it.
