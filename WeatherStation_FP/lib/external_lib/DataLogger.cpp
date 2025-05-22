#include "DataLogger.h"
#include "esp_log.h"

void DataLogger::logEspLog(const char* msg1, const char* msg2) {
            ESP_LOGE(msg1, "%s", msg2);
        }