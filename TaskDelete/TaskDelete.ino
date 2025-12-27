// for FreeRTOS-based Arduino cores, e.g. ESP32

// This program demonstrates how multiple 'vTaskDelete(NULL)' calls cause 'xTaskCreate' failure
// due to RTOS memory overflow in case RTOS Idle task is starving (i.e. not called in this case
// because it's priority is 0). Uncommenting 'vTaskDelay' in TestTask can fix the issue but the
// current version of the program demonstrates another approach (using vTaskSuspend(NULL) /
// vTaskDelete(<task handle>)) that seems to be more reliable (nethertheless the recommendation is
// to avoid dynamic task creation and deletion if possible).

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  while (!Serial);
  delay(1000);
  Serial.printf("Setup task handle: %p\n", xTaskGetCurrentTaskHandle());
}

TaskHandle_t xLoopTask;

void TestTask(void* pvParameters)
{
  Serial.printf("%u TestTask starts\n", millis());

//  taskYIELD(); // still crashes
/*
  // doesn't crash - Idle task is called
  vTaskDelay(pdMS_TO_TICKS(1));
*/
  Serial.printf("%u TestTask finishes\n", millis());

  xTaskNotifyGive(xLoopTask);

  // just for test
//  delay(500);
  Serial.printf("%u TestTask suspends itself\n", millis());

  vTaskSuspend(NULL);

//  vTaskDelete(NULL); // Idle task must run and free memory
}

void loop()
{
  if (!xLoopTask)
  {
    xLoopTask = xTaskGetCurrentTaskHandle();
    Serial.printf("Loop task handle: %p\n", xTaskGetCurrentTaskHandle());
  }

  static uint counter;

  TaskHandle_t xTestTaskHandle;

  if (xTaskCreate(
    TestTask,   // Function to implement the task
    "TestTask", // Name of the task
    4096,       // Stack size in words (not bytes!)
    NULL,       // Task input parameter
    1,          // Priority of the task
    &xTestTaskHandle // Task handle
  ) == pdTRUE)
  {
    Serial.printf(F("%u TestTask(%u) started: %p\n"), millis(), counter, xTestTaskHandle);
  }
  else
  {
    log_e("xTaskCreate failed");
    for (;;) ;
  }

  ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for notification

  Serial.printf(F("%u TestTask(%u) notified its finishing\n"), millis(), counter);

  // wait until the task gets suspended
  while (eTaskGetState(xTestTaskHandle) != eSuspended) ;

  vTaskDelete(xTestTaskHandle);

  Serial.printf(F("%u TestTask(%u) deleted\n"), millis(), counter);

  ++counter;
}
