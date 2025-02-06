#ifndef WIFI_H
#define WIFI_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

class WiFiManager {
public:
    WiFiManager();
    void start();

private:
    
};


#endif // WIFI_H