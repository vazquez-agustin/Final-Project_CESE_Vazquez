#include "main.h"

static const char *TAG = "WiFi";

// WiFi credentials
#define WIFI_SSID "FRITZ!Box 7530 KW"
#define WIFI_PASS "43985539165212248477"

// WiFi events handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
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

// WiFi initialization
void wifi_init_sta() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Registrar manejadores de eventos para WiFi e IP
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    // Configuración de WiFi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); // Modo estación (conexión a red existente)
    ESP_ERROR_CHECK(esp_wifi_set_config((wifi_interface_t)ESP_IF_WIFI_STA, &wifi_config)); // Aplicar la configuración
    ESP_ERROR_CHECK(esp_wifi_start()); // Iniciar WiFi

    ESP_LOGI(TAG, "WiFi inicializado. Intentando conectar...");
}

extern "C" void app_main() {
    // Tu código inicial aquí.
    printf("¡Hola Mundo! ESP32-C3 está funcionando\n");

    // Inicializar almacenamiento no volátil (necesario para WiFi)
    ESP_ERROR_CHECK(nvs_flash_init());

    // Inicializar WiFi
    wifi_init_sta();
}