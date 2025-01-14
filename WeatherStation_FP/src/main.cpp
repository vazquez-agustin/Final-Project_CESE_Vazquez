#include "main.h"

static const char *TAG = "WIFI_CONNECTION";

// Configuración de WiFi
#define WIFI_SSID "TU_SSID"
#define WIFI_PASS "TU_PASSWORD"

static void wifi_event_handler(void* arg, esp_event_base_t event_base, 
                                int32_t event_id, void* event_data)
{
    switch(event_id) {
        case WIFI_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "Conectado a %s", WIFI_SSID);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Desconectado de %s, intentando reconectar...", WIFI_SSID);
            esp_wifi_connect();
            break;
        default:
            break;
    }
}

static void ip_event_handler(void* arg, esp_event_base_t event_base, 
                                int32_t event_id, void* event_data)
{
    switch(event_id) {
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "Obtenido IP");
            break;
        default:
            break;
    }
}

void connect_to_wifi() {
    // Inicializar el subsistema WiFi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // Configuración WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Registro de eventos WiFi
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));

    // Configuración de la estación WiFi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main(void)
{
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Conectando a %s...", WIFI_SSID);
    connect_to_wifi();

    // Tu lógica principal aquí después de la conexión WiFi
}