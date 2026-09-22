// FreeRTOS Scheduler Demonstration
// ESP32-S3
// Task A: Priority 1
// Task B: Priority 2

TaskHandle_t taskAHandle;
TaskHandle_t taskBHandle;

// -------------------------
// Task A
// -------------------------
void taskA(void *parameter)
{
  while (true)
  {
    Serial.println("\nTask A is printing slowly...");

    Serial.print("Task A: ");
    Serial.flush();

    // Print character by character
    const char *message = "Hello from Task A";

    for (int i = 0; message[i] != '\0'; i++)
    {
      Serial.print(message[i]);
      Serial.flush();

      // Small delay so we can observe the scheduler
      vTaskDelay(pdMS_TO_TICKS(100));

      // Notify Task B during Task A's message
      if (i == 5)
      {
        Serial.println("\nTask A: notifying Task B");
        xTaskNotifyGive(taskBHandle);
      }
    }

    Serial.println();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// -------------------------
// Task B
// -------------------------
void taskB(void *parameter)
{
  while (true)
  {
    // Wait for Task A notification
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    Serial.println(">>> Task B is running!");
    Serial.println(">>> Task B received notification.");
  }
}

// -------------------------
// Setup
// -------------------------
void setup()
{
  Serial.begin(115200);

  // Give Serial time to start
  vTaskDelay(pdMS_TO_TICKS(1000));

  Serial.println("\n=== FreeRTOS Scheduler Demo ===");

  // Both tasks are pinned to the SAME core
  xTaskCreatePinnedToCore(
    taskA,          // Task function
    "Task A",       // Task name
    4096,           // Stack size
    NULL,           // Parameter
    1,              // Priority
    &taskAHandle,   // Task handle
    0               // Core 0
  );

  xTaskCreatePinnedToCore(
    taskB,          // Task function
    "Task B",       // Task name
    4096,           // Stack size
    NULL,           // Parameter
    3,              // Priority
    &taskBHandle,   // Task handle
    0               // Core 0
  );
}

void loop()
{
  // Nothing needed here.
  vTaskDelay(pdMS_TO_TICKS(1000));
}