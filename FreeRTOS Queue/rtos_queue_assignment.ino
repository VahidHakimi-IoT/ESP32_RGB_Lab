
#include <Arduino.h>

// Declare the queue
QueueHandle_t temperatureQueue;


// --------------------------------------------------
// Sensor Task
// --------------------------------------------------

void sensorTask(void *parameter)
{
  int temperature = 20;

  while (1)
  {
    // Send temperature to queue
    xQueueSend(temperatureQueue, &temperature, portMAX_DELAY);

    // Increase temperature
    temperature++;

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


// --------------------------------------------------
// Display Task
// --------------------------------------------------

void displayTask(void *parameter)
{
  int receivedTemperature;

  while (1)
  {
    // Receive temperature from queue
    if (xQueueReceive(temperatureQueue, &receivedTemperature, portMAX_DELAY) == pdPASS)
    {
      Serial.print("Temperature: ");
      Serial.print(receivedTemperature);
      Serial.println(" C");
    }
  }
}


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
  Serial.begin(115200);

  // Create queue for 5 integers
  temperatureQueue = xQueueCreate(5, sizeof(int));

  if (temperatureQueue == NULL)
  {
    Serial.println("Queue creation failed!");
    return;
  }

  // Create Sensor Task
  xTaskCreate(
    sensorTask,
    "Sensor Task",
    2048,
    NULL,
    1,
    NULL
  );

  // Create Display Task
  xTaskCreate(
    displayTask,
    "Display Task",
    2048,
    NULL,
    1,
    NULL
  );
}


// --------------------------------------------------
// Loop
// --------------------------------------------------

void loop()
{
}
