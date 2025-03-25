#ifndef WIFI_H
#define WIFI_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>
#include <string>

class WiFiManager {
private:
    static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    void handle_event(esp_event_base_t event_base, int32_t event_id, void* event_data);
    bool connected = false;  // Inicialmente false, se pone true al obtener IP

public:
    WiFiManager();
    void initialization(std::string ssid, std::string password);
    bool isConnected() const { return connected; }
};


#endif // WIFI_H
