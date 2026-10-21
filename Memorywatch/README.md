# ESP32-S3 Memory Watch — Task Stack Size Experiment

**Student:** Vahid Hakimi  
**Project:** Memory Watch — FreeRTOS  
**Board:** ESP32-S3 Dev Module

## 1. Measurements

### Run 1: Stack size = 4096 bytes

| Measurement | Free heap |
|---|---:|
| Before Task A | 359,112 bytes |
| After Task A | 354,384 bytes |
| After Task B | 349,656 bytes |

### Run 2: Stack size = 8192 bytes

| Measurement | Free heap |
|---|---:|
| Before Task A | 359,112 bytes |
| After Task A | 350,032 bytes |
| After Task B | 341,208 bytes |

## 2. Observations and Answers

**a. Did the free heap change when the task stack size increased?**

Yes. The free heap decreased more when the stack size increased from 4096 to 8192 bytes.

**b. What happened to the free heap?**

With 4096-byte stacks, the total heap decrease after creating both tasks was 9,456 bytes. With 8192-byte stacks, it was 17,904 bytes. The larger stacks consumed an additional 8,448 bytes of heap.

**c. Why does a task need stack memory?**

A task needs stack memory to store local variables, function parameters, return addresses, and information required during function calls and execution.

**d. Why does creating a FreeRTOS task use RAM?**

Creating a task uses RAM because FreeRTOS allocates memory for its stack and task-control information. Increasing the stack size reserves more memory for each task, leaving less free heap for other operations.

## 3. Evidence

Attach Serial Monitor screenshots showing the measurements for both stack sizes.

## 4. GitHub Repository

