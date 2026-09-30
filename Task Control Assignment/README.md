# FreeRTOS Task Control – ESP32-S3

## Overview

In this assignment, I created two independent FreeRTOS tasks on an ESP32-S3-DevKitC-1:

- **Serial Task** – reads commands from the Serial Monitor and changes the LED blink interval or suspends/resumes the LED Task.
- **RGB LED Task** – controls the onboard RGB LED and blinks it using the selected interval.

Both tasks are pinned to **Core 0**.

The available commands are:

- `250` → blink every 250 ms
- `500` → blink every 500 ms
- `1000` → blink every 1000 ms
- `suspend` → suspend the LED Task
- `resume` → resume the LED Task

## Hardware and Software

### Hardware

- ESP32-S3-DevKitC-1 v1.1
- Onboard RGB LED
- RGB LED data pin: GPIO 38
- USB connection to computer

### Software

- Arduino IDE 2.x
- ESP32 boards package by Espressif Systems
- Adafruit NeoPixel library
- Serial Monitor: 115200 baud
- FreeRTOS included with the ESP32 Arduino framework

## Task Design

### Serial Task

The Serial Task continuously checks for commands from the Serial Monitor.

For valid interval commands, it updates the shared `blinkInterval` variable.

For `suspend`, it uses:

```cpp
vTaskSuspend(ledTaskHandle);
```

For `resume`, it uses:

```cpp
vTaskResume(ledTaskHandle);
```

The Serial Task also uses `vTaskDelay()` so that it does not continuously occupy the CPU.

### RGB LED Task

The LED Task controls the onboard RGB LED. It turns the LED on, waits for the selected interval, turns it off, and waits again.

The LED Task reads the shared `blinkInterval` value when setting its delays.

## Shared Variable

The two tasks use:

```cpp
volatile uint32_t blinkInterval = 500;
```

The initial blink interval is **500 ms**.

The Serial Task changes this value, while the LED Task reads it.

The `volatile` keyword is used because the value can be changed by another task.

## Task Handles

The program uses two task handles:

```cpp
TaskHandle_t serialTaskHandle = NULL;
TaskHandle_t ledTaskHandle = NULL;
```

The LED Task handle allows the Serial Task to suspend and resume the LED Task.

## Task Creation and Core Assignment

Both tasks are created using `xTaskCreatePinnedToCore()` and are pinned to **Core 0**.

Both tasks have priority **1**.

## Prediction

Before testing, I predicted:

| Situation | Prediction |
|---|---|
| LED Task running normally | LED blinks and Serial accepts commands |
| LED Task suspended | LED stops, but Serial continues working |
| LED Task resumed | LED Task becomes Ready and starts running again when scheduled |

The prediction was confirmed by the practical test.

## FreeRTOS Task States

### Running

A task is **Running** when it is currently executing on the CPU.

### Ready

A task is **Ready** when it is able to run but is waiting for the scheduler to give it CPU time.

### Blocked

A task becomes **Blocked** when it is waiting for something, such as a time delay.

For example:

```cpp
vTaskDelay(pdMS_TO_TICKS(blinkInterval));
```

causes the LED Task to become Blocked for the selected time.

### Suspended

A task becomes **Suspended** when it is explicitly suspended using `vTaskSuspend()`.

A suspended task cannot run until another task resumes it.

## Task-State Analysis

### Normal Operation

During normal operation, both tasks are active.

The LED Task repeatedly changes the LED state and becomes Blocked during `vTaskDelay()`.

The Serial Task also periodically becomes Blocked while waiting for new input.

The scheduler allows both tasks to execute.

### LED Task Suspended

When the command:

```text
suspend
```

is entered, the Serial Task calls:

```cpp
vTaskSuspend(ledTaskHandle);
```

The LED Task enters the **Suspended** state and stops running.

The Serial Task continues working independently.

### LED Task Resumed

When:

```text
resume
```

is entered, the Serial Task calls:

```cpp
vTaskResume(ledTaskHandle);
```

The LED Task changes from **Suspended** to **Ready**. The scheduler can then run it again.

After running, the LED Task can become Blocked again when it calls `vTaskDelay()`.

## Testing and Results

I tested the program using this sequence:

```text
500
suspend
250
resume
1000
```

### Test 1 – 500 ms

I entered:

```text
500
```

The LED blinked normally.

**Result: Passed.**

### Test 2 – Suspend

I entered:

```text
suspend
```

The LED stopped blinking, while the Serial Task continued working.

**Result: Passed.**

### Test 3 – 250 ms while suspended

While the LED Task was suspended, I entered:

```text
250
```

The Serial Monitor accepted the command successfully.

The LED remained stopped because the LED Task was still suspended.

**Result: Passed.**

### Test 4 – Resume

I entered:

```text
resume
```

The LED started blinking again.

**Result: Passed.**

### Test 5 – 1000 ms

Finally, I entered:

```text
1000
```

The command worked and the LED continued operating with the new interval.

**Result: Passed.**

## Final Test Sequence

The complete successful test was:

```text
500
    ↓
LED blinking normally

suspend
    ↓
LED stopped
Serial still working

250
    ↓
Serial accepted the command
LED remained suspended

resume
    ↓
LED started again

1000
    ↓
LED continued working with the new interval
```

## Reflection

During this assignment, I learned how FreeRTOS can be used to divide a program into independent tasks. I created a Serial Task for receiving commands and an RGB LED Task for controlling the LED. Both tasks were pinned to the same CPU core, which helped me understand how the FreeRTOS scheduler manages multiple tasks.

One important observation was that suspending the LED Task did not stop the Serial Task. After entering `suspend`, the LED stopped blinking, but I was still able to enter `250` through the Serial Monitor. This showed that the two tasks operate independently.

I also learned the difference between the Suspended and Blocked states. The LED Task normally becomes Blocked during `vTaskDelay()`, while `vTaskSuspend()` puts it into the Suspended state until it is explicitly resumed.

After entering `resume`, the LED started working again. Finally, changing the interval to `1000` ms also worked successfully. The practical test confirmed my predictions about task scheduling, suspension, resumption, and shared data.

## Submission Files

The project contains:

```text
src/
└── main.cpp

README.md
```

The submission also includes screenshots/photos showing:

1. Normal LED operation.
2. LED suspended while the Serial Task continues working.
3. LED resumed and operating again.

## AI Usage

**Tool used:** ChatGPT

**How I used it:** I used ChatGPT to help understand FreeRTOS concepts, task states, task suspension/resumption, shared variables, and the structure of the ESP32 code.

**What I verified or changed:** I uploaded and tested the code on my ESP32-S3-DevKitC-1. I tested the commands `500`, `suspend`, `250`, `resume`, and `1000` using the Serial Monitor. I also checked the physical LED behavior and confirmed that the LED stopped when suspended and started again after resuming.
