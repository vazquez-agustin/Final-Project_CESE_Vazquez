#include "main.h"
#include <iostream>

// WiFi credentials
#define WIFI_SSID "FRITZ!Box 7530 KW"
#define WIFI_PASS "43985539165212248477"

// InfluxDB local server URL
#define INFLUXDB_URL "http://192.168.0.100:8086/api/v2/write"

// InfluxDB credentials and configuration
#define INFLUXDB_BUCKET "meditions"
#define INFLUXDB_ORG "stationtest"
#define INFLUXDB_TOKEN "vND8qiFkSJqwD0kZqPAGilY16z3fTexXVbOlYb968a6wFsGj9MT5JCkcI3aforCHT9ncyz33RE3gYOKx20Rccw=="

// WiFi events handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Intentando reconexión al WiFi");
        esp_wifi_connect();
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WiFi conectado");
    }
}

// WiFi initialization
void wifi_init_sta() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}