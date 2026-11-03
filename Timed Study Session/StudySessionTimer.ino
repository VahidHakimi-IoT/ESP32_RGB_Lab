
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// ========== HARDWARE SETTINGS ==========

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

#define SDA_PIN 8
#define SCL_PIN 9

// Change to 38 if your board is revision V1.1
#define RGB_LED_PIN 38
#define RGB_LED_COUNT 1

#define SESSION_DURATION_MS 10000UL

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

Adafruit_NeoPixel rgbLed(
  RGB_LED_COUNT,
  RGB_LED_PIN,
  NEO_GRB + NEO_KHZ800
);

// ========== FREERTOS SETTINGS ==========

#define UPDATE_BIT   (1UL << 0)
#define COMPLETE_BIT (1UL << 1)

TimerHandle_t updateTimer = NULL;
TimerHandle_t completionTimer = NULL;

TaskHandle_t appTaskHandle = NULL;

// ========== SESSION VARIABLES ==========

bool sessionActive = false;
uint32_t sessionStartTime = 0;

// ========== LED FUNCTIONS ==========

void ledOn() {
  rgbLed.setPixelColor(0, rgbLed.Color(0, 255, 0));
  rgbLed.show();
}

void ledOff() {
  rgbLed.clear();
  rgbLed.show();
}

// ========== OLED FUNCTIONS ==========

void showReady() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 5);
  display.println("Ready");

  display.setTextSize(1);
  display.setCursor(0, 35);
  display.println("Send s to start");

  display.display();
}

void showRunning(int remaining) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 5);
  display.println("Session");

  display.setCursor(0, 25);
  display.println("running");

  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Remaining: ");
  display.print(remaining);
  display.println(" s");

  display.display();
}

void showComplete() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 10);
  display.println("Session");

  display.setCursor(0, 35);
  display.println("complete");

  display.display();
}

// ========== TIMER CALLBACKS ==========

// Auto-reload timer callback.
// Only sends an UPDATE notification.

void updateTimerCallback(TimerHandle_t timer) {

  if (appTaskHandle != NULL) {
    xTaskNotify(
      appTaskHandle,
      UPDATE_BIT,
      eSetBits
    );
  }
}

// One-shot timer callback.
// Only sends a COMPLETE notification.

void completionTimerCallback(TimerHandle_t timer) {

  if (appTaskHandle != NULL) {
    xTaskNotify(
      appTaskHandle,
      COMPLETE_BIT,
      eSetBits
    );
  }
}

// ========== START SESSION ==========

void startSession() {

  if (sessionActive) {
    return;
  }

  // Clear any old notification bits.
  uint32_t oldEvents = 0;

  xTaskNotifyWait(
    UPDATE_BIT | COMPLETE_BIT,
    UPDATE_BIT | COMPLETE_BIT,
    &oldEvents,
    0
  );

  sessionStartTime = millis();
  sessionActive = true;

  ledOn();
  showRunning(10);

  Serial.println("Session started!");

  // Start the auto-reload timer.
  BaseType_t updateResult = xTimerStart(
    updateTimer,
    0
  );

  // Start the one-shot timer.
  BaseType_t completionResult = xTimerStart(
    completionTimer,
    0
  );

  // Check timer commands.
  if (
    updateResult != pdPASS ||
    completionResult != pdPASS
  ) {

    Serial.println("ERROR: Timer start failed!");

    sessionActive = false;

    xTimerStop(updateTimer, 0);
    xTimerStop(completionTimer, 0);

    ledOff();
    showReady();

    return;
  }
}

// ========== APPLICATION TASK ==========

void applicationTask(void *parameter) {

  uint32_t events = 0;

  while (true) {

    events = 0;

    // Wait for a maximum of 20 ms.
    // This keeps Serial responsive.

    xTaskNotifyWait(
      0,
      UPDATE_BIT | COMPLETE_BIT,
      &events,
      pdMS_TO_TICKS(20)
    );

    // COMPLETION HAS PRIORITY.

    if (events & COMPLETE_BIT) {

      if (sessionActive) {

        sessionActive = false;

        BaseType_t stopResult = xTimerStop(
          updateTimer,
          0
        );

        if (stopResult != pdPASS) {
          Serial.println("WARNING: Update timer stop failed!");
        }

        ledOff();
        showComplete();

        Serial.println("Session complete!");
      }

      // Do not process an update from
      // the same notification batch.

      events &= ~UPDATE_BIT;
    }

    // PROCESS COUNTDOWN UPDATE.

    else if (events & UPDATE_BIT) {

      if (sessionActive) {

        uint32_t elapsed =
          millis() - sessionStartTime;

        if (elapsed < SESSION_DURATION_MS) {

          // Calculate remaining time from
          // actual elapsed milliseconds.

          uint32_t remaining =
            (SESSION_DURATION_MS - elapsed + 999) / 1000;

          showRunning((int)remaining);
        }
      }
    }

    // READ SERIAL COMMANDS.

    while (Serial.available() > 0) {

      char command = Serial.read();

      if (command == 's' && !sessionActive) {
        startSession();
      }

      // Other characters and line endings
      // are automatically ignored.
    }
  }
}

// ========== SETUP ==========

void setup() {

  Serial.begin(115200);

  // Initialize I2C.
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED.
  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
      )) {

    Serial.println("ERROR: OLED initialization failed!");

    while (true) {
      delay(1000);
    }
  }

  // Initialize built-in RGB LED.
  rgbLed.begin();
  rgbLed.setBrightness(40);
  rgbLed.clear();
  rgbLed.show();

  ledOff();

  // Show initial screen.
  showReady();

  Serial.println("Study Session Timer Ready.");
  Serial.println("Send 's' to start.");

  // CREATE AUTO-RELOAD TIMER.

  updateTimer = xTimerCreate(
    "UpdateTimer",
    pdMS_TO_TICKS(1000),
    pdTRUE,
    NULL,
    updateTimerCallback
  );

  // CREATE ONE-SHOT TIMER.

  completionTimer = xTimerCreate(
    "CompletionTimer",
    pdMS_TO_TICKS(10000),
    pdFALSE,
    NULL,
    completionTimerCallback
  );

  // CHECK TIMER CREATION.

  if (
    updateTimer == NULL ||
    completionTimer == NULL
  ) {

    Serial.println("ERROR: Timer creation failed!");

    while (true) {
      delay(1000);
    }
  }

  Serial.println("Both timers created successfully.");

  // CREATE APPLICATION TASK.

  BaseType_t taskResult = xTaskCreate(
    applicationTask,
    "ApplicationTask",
    4096,
    NULL,
    1,
    &appTaskHandle
  );

  if (taskResult != pdPASS) {

    Serial.println("ERROR: Application task creation failed!");

    while (true) {
      delay(1000);
    }
  }
}

// ========== LOOP ==========

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
