// ESP32-S3, Arduino IDE 2.x, Serial Monitor 115200. No external wiring.
// Final version: consumer delay 1500 ms (test 1 used 400 ms), timestamps on consumer lines.
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t DEMO_CORE = 0;
#else
static const BaseType_t DEMO_CORE = 1;
#endif

void fatal(const char *message) {
  Serial.println(message);
  while (true) { vTaskDelay(pdMS_TO_TICKS(1000)); }
}

void startTask(TaskFunction_t fn, const char *name, void *arg) {
  if (xTaskCreatePinnedToCore(fn, name, 4096, arg, 1, nullptr, DEMO_CORE) != pdPASS)
    fatal("ERROR: task creation failed. Reset after checking available memory.");
}

static const int CAPACITY = 5, PRODUCERS = 5, WRITES = 3, CONSUMERS = 2;
static const uint32_t PRODUCER_DELAY_MS = 100, CONSUMER_DELAY_MS = 1500;  // test 1 was 400
static const uint32_t TEST_TIMEOUT_MS = 30000;

static int buffer[CAPACITY], head = 0, tail = 0, consumed = 0;
static int counts[PRODUCERS] = {0}, invalidValues = 0;
static int producerIds[PRODUCERS] = {0, 1, 2, 3, 4};
static int consumerIds[CONSUMERS] = {0, 1};

SemaphoreHandle_t emptySlots, filledSlots, bufferMutex, allConsumed;

bool createObjects() {
  emptySlots  = xSemaphoreCreateCounting(CAPACITY, CAPACITY);  // 5 free places at start
  filledSlots = xSemaphoreCreateCounting(CAPACITY, 0);         // 0 ready values at start
  bufferMutex = xSemaphoreCreateMutex();
  allConsumed = xSemaphoreCreateBinary();                      // provided completion signal
  return emptySlots && filledSlots && bufferMutex && allConsumed;
}

void putValue(int value) {
  xSemaphoreTake(emptySlots, portMAX_DELAY);    // 1. reserve a free place
  xSemaphoreTake(bufferMutex, portMAX_DELAY);   // 2. exclusive access
  buffer[head] = value;                         // 3. provided write
  head = (head + 1) % CAPACITY;
  xSemaphoreGive(bufferMutex);                  // 4. release access
  xSemaphoreGive(filledSlots);                  // 5. announce a ready value
}

void getValue(int consumerId) {
  xSemaphoreTake(filledSlots, portMAX_DELAY);   // 1. claim a ready value
  xSemaphoreTake(bufferMutex, portMAX_DELAY);   // 2. exclusive access
  int value = buffer[tail];                     // 3. provided read
  tail = (tail + 1) % CAPACITY;
  if (value >= 0 && value < PRODUCERS) ++counts[value]; else ++invalidValues;
  ++consumed;
  Serial.printf("[%lu ms] Consumer %d read %d (total=%d)\n",
                (unsigned long)millis(), consumerId, value, consumed);
  if (consumed == PRODUCERS * WRITES) xSemaphoreGive(allConsumed);
  xSemaphoreGive(bufferMutex);                  // 4. release access
  xSemaphoreGive(emptySlots);                   // 5. announce a free place
}

void producer(void *arg) {
  const int id = *static_cast<int *>(arg);
  for (int i = 0; i < WRITES; ++i) {
    putValue(id);
    vTaskDelay(pdMS_TO_TICKS(PRODUCER_DELAY_MS));
  }
  vTaskDelete(nullptr);
}

void consumer(void *arg) {
  const int id = *static_cast<int *>(arg);
  while (true) {
    getValue(id);
    vTaskDelay(pdMS_TO_TICKS(CONSUMER_DELAY_MS));  // outside the mutex
  }
}

void monitor(void *) {
  if (xSemaphoreTake(allConsumed, pdMS_TO_TICKS(TEST_TIMEOUT_MS)) != pdTRUE) {
    Serial.println("TIMEOUT: expected 15 consumed values. Check waits and Give calls.");
    vTaskDelete(nullptr);
  }
  if (xSemaphoreTake(bufferMutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
    Serial.println("ERROR: completion signal received but mutex unavailable");
    vTaskDelete(nullptr);
  }
  bool pass = (consumed == PRODUCERS * WRITES && invalidValues == 0);
  Serial.printf("TOTAL=%d expected=%d invalid=%d\n", consumed, PRODUCERS * WRITES, invalidValues);
  for (int i = 0; i < PRODUCERS; ++i) {
    Serial.printf("Value %d: count=%d expected=%d\n", i, counts[i], WRITES);
    if (counts[i] != WRITES) pass = false;
  }
  Serial.println(pass ? "PASS: total and all frequencies correct" : "FAIL: inspect values and counts");
  xSemaphoreGive(bufferMutex);
  vTaskDelete(nullptr);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("Semaphore buffer challenge (consumer delay 1500 ms)");
  if (!createObjects()) fatal("STARTER: complete TODOs before running. No worker tasks started.");
  for (int i = 0; i < CONSUMERS; ++i) startTask(consumer, "consumer", &consumerIds[i]);
  for (int i = 0; i < PRODUCERS; ++i) startTask(producer, "producer", &producerIds[i]);
  startTask(monitor, "monitor", nullptr);
}

void loop() { vTaskDelay(pdMS_TO_TICKS(1000)); }
