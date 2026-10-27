A FreeRTOS bounded buffer (5 places) shared by 5 producer tasks and 2 consumer tasks. Two counting semaphores and one mutex coordinate the tasks. Code: `semaphore_buffer/semaphore_buffer.ino`. Log: `serial_log.txt`.

## Synchronisation objects

| Object | Type | Max | Initial |
|---|---|---|---|
| `emptySlots` | counting semaphore | 5 | 5 |
| `filledSlots` | counting semaphore | 5 | 0 |
| `bufferMutex` | mutex | | available |

`emptySlots` starts at 5 and `filledSlots` at 0 because the buffer starts empty: all five places are free and no value is ready to read. All three objects are checked for NULL before any worker task starts.

## Questions

**1. What does `emptySlots` count?**
The number of free places left in the buffer. A producer must take one before writing, and a consumer gives one back after reading. When it is 0 the buffer is full.

**2. What does `filledSlots` count?**
The number of written values waiting to be read. A producer gives one after writing, and a consumer must take one before reading. When it is 0 the buffer is empty.

**3. What does the mutex protect?**
The shared buffer data: the `buffer` array, the `head` and `tail` positions, and the shared counters (`consumed`, `counts`). The mutex makes sure only one task changes them at a time. The semaphores decide *when* a task may go; the mutex decides *who* touches the data right now.

**4. What did you change in the second test, and what happened?**
I changed `CONSUMER_DELAY_MS` from 400 to 1500 and added timestamps to the consumer lines. The delay is in `consumer()` after `getValue()` returns, so it is outside the mutex-protected section.

*Prediction (written before running):* if the buffer becomes full, `emptySlots` is 0, so each producer blocks at `xSemaphoreTake(emptySlots, portMAX_DELAY)` inside `putValue()` until a consumer calls `xSemaphoreGive(emptySlots)` at the end of `getValue()`.

*Result:* the run still ended with `TOTAL=15 expected=15 invalid=0`, each ID 0-4 counted 3 times, and `PASS`. Consumers read in pairs about 1500 ms apart (1507, 3006, 4506 ... 12006 ms), so the whole run took about 12 s instead of about 3-4 s. The output does not print a task waiting. The call that blocks when the buffer is full is the `emptySlots` Take in `putValue()`: producers write every 100 ms but consumers free a place only about every 1500 ms, so the buffer fills and producers wait there. The data stayed correct, so the blocking worked as intended.

## One producer operation and one consumer operation

*Producer (`putValue`):* take `emptySlots` to reserve a free place (blocks if full), take `bufferMutex`, write the value and advance `head`, give `bufferMutex`, give `filledSlots` to announce a ready value.

*Consumer (`getValue`):* take `filledSlots` to claim a ready value (blocks if empty), take `bufferMutex`, read the value and advance `tail`, give `bufferMutex`, give `emptySlots` to announce a free place.

The semaphore Take comes before the mutex Take in both. If a task took the mutex first and then waited on a semaphore, it would hold the mutex while sleeping and could deadlock the others.

## Test results

| Test | Consumer delay | Consumed | Each ID 0-4 | Result |
|---|---|---|---|---|
| 1 | 400 ms | 15 | 3 times | PASS |
| 2 | 1500 ms | 15 | 3 times | PASS |

## AI assistance

I used Claude to explain the producer/consumer ordering, to help fill in the TODOs in the starter code, and to troubleshoot upload and Serial Monitor problems (duplicate `.ino` files, wrong code on the board). I checked the result by uploading to the ESP32-S3, reading the Serial Monitor output myself, counting the 15 consumer lines, confirming each ID appears 3 times, and confirming the `PASS` line and the 1500 ms timestamp gaps in test 2.


