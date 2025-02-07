#include "WiFi.h"


static const char* TAG = "WiFiManager";

#define WIFI_SSID "FRITZ!Box 7530 KW"
#define WIFI_PASS "43985539165212248477"

WiFiManager::WiFiManager() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
}

void WiFiManager::start() {
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFiManager::wifi_event_handler, this);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFiManager::wifi_event_handler, this);

    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password));

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

void WiFiManager::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    WiFiManager* instance = static_cast<WiFiManager*>(arg);
    instance->handle_event(event_base, event_id, event_data);
}

void WiFiManager::handle_event(esp_event_base_t event_base, int32_t event_id, void* event_data) {
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
