#include "WiFi.h"

static const char* TAG = "WiFiManager";

#define WIFI_SSID "FRITZ!Box 7530 KW"
#define WIFI_PASS "43985539165212248477"

WIFI::WiFiManager() {
        esp_netif_init();
        esp_event_loop_create_default();
        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_wifi_init(&cfg);
}

void WIFI::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
        WiFiManager* instance = static_cast<WiFiManager*>(arg);
        instance->handle_event(event_base, event_id, event_data);
}

void WIFI::handle_event(esp_event_base_t event_base, int32_t event_id, void* event_data) {
        if (event_id == WIFI_EVENT_STA_START) {
            ESP_LOGI(TAG, "Conectando al WiFi...");
            esp_wifi_connect();
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            ESP_LOGE(TAG, "WiFi desconectado. Reintentando...");
            esp_wifi_connect();
        } else if (event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "WiFi conectado. Dirección IP: " IPSTR, IP2STR(&event->ip_info.ip));
        }
}