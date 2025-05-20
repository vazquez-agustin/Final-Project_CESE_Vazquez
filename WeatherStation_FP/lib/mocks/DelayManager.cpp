#include "DelayManager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void DelayManager::delay(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}