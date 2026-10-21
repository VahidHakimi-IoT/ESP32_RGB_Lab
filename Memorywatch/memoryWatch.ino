
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t taskAHandle = NULL;
TaskHandle_t taskBHandle = NULL;

void printFreeHeap(const char* message)
{
  Serial.print(message);
  Serial.print(": ");
  Serial.print(xPortGetFreeHeapSize());
  Serial.println(" bytes");
}

void taskA(void *parameter)
{
  while (1)
  {
    Serial.println("Task A is running");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void taskB(void *parameter)
{
  while (1)
  {
    Serial.println("Task B is running");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== RTOS Memory Demo ===");

  // 1. Check memory before creating our tasks
  printFreeHeap("Free heap before tasks");

  // 2. Create Task A
  Serial.println();
  Serial.println("Creating Task A...");

  BaseType_t resultA = xTaskCreate(
    taskA,
    "TaskA",
    8192,
    NULL,
    1,
    &taskAHandle
  );

  if (resultA == pdPASS)
  {
    Serial.println("Task A created.");
  }
  else
  {
    Serial.println("Task A creation FAILED.");
  }

  printFreeHeap("Free heap after Task A");

  delay(1000);

  // 3. Create Task B
  Serial.println();
  Serial.println("Creating Task B...");

  BaseType_t resultB = xTaskCreate(
    taskB,
    "TaskB",
    8192,
    NULL,
    1,
    &taskBHandle
  );

  if (resultB == pdPASS)
  {
    Serial.println("Task B created.");
  }
  else
  {
    Serial.println("Task B creation FAILED.");
  }

  printFreeHeap("Free heap after Task B");

  Serial.println();
  Serial.println("=== Demo finished ===");
}

void loop()
{
  delay(1000);
}
