#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// ===============================
// RGB LED
// ===============================

#define LED_PIN 38
#define NUM_LEDS 1

Adafruit_NeoPixel pixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ===============================
// Shared variable
// ===============================

volatile uint32_t blinkInterval = 500;

// ===============================
// Task handles
// ===============================

TaskHandle_t serialTaskHandle = NULL;
TaskHandle_t ledTaskHandle = NULL;

// ===============================
// Serial Task
// ===============================

void serialTask(void *parameter)
{
  for (;;)
  {
    if (Serial.available())
    {
      String input = Serial.readStringUntil('\n');
      input.trim();

      Serial.print("Received: ");
      Serial.println(input);

      if (input == "250")
      {
        blinkInterval = 250;
        Serial.println("Blink interval changed to 250 ms");
      }
      else if (input == "500")
      {
        blinkInterval = 500;
        Serial.println("Blink interval changed to 500 ms");
      }
      else if (input == "1000")
      {
        blinkInterval = 1000;
        Serial.println("Blink interval changed to 1000 ms");
      }
      else if (input == "suspend")
      {
        vTaskSuspend(ledTaskHandle);
        Serial.println("LED Task suspended");
      }
      else if (input == "resume")
      {
        vTaskResume(ledTaskHandle);
        Serial.println("LED Task resumed");
      }
      else
      {
        Serial.println("Invalid input.");
        Serial.println("Use 250, 500, 1000, suspend or resume.");
      }
    }

    // Allow the Serial Task to block briefly
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

// ===============================
// LED Task
// ===============================

void ledTask(void *parameter)
{
  for (;;)
  {
    // LED ON
    pixel.setPixelColor(0, pixel.Color(0, 255, 0));
    pixel.show();

    vTaskDelay(pdMS_TO_TICKS(blinkInterval));

    // LED OFF
    pixel.setPixelColor(0, pixel.Color(0, 0, 0));
    pixel.show();

    vTaskDelay(pdMS_TO_TICKS(blinkInterval));
  }
}

// ===============================
// Setup
// ===============================

void setup()
{
  Serial.begin(115200);

  pixel.begin();
  pixel.clear();
  pixel.show();

  Serial.println("FreeRTOS Task Control Assignment");
  Serial.println("Commands:");
  Serial.println("250 / 500 / 1000");
  Serial.println("suspend");
  Serial.println("resume");
  Serial.println();

  // ===============================
  // Create Serial Task
  // ===============================

  xTaskCreatePinnedToCore(
    serialTask,
    "Serial Task",
    4096,
    NULL,
    1,
    &serialTaskHandle,
    0
  );

  // ===============================
  // Create LED Task
  // ===============================

  xTaskCreatePinnedToCore(
    ledTask,
    "LED Task",
    4096,
    NULL,
    1,
    &ledTaskHandle,
    0
  );
}

// ===============================
// Main loop
// ===============================

void loop()
{
  // Nothing needed here.
}