#include "esp_log.h"
#include "DataLogger.h"

void DataLogger::logEspLog(char* msg1, char* msg2) {
            ESP_LOGE(msg1, "%s", msg2);
        }