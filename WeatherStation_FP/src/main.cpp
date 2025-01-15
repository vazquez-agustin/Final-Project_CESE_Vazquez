#include "main.h"

static const char *TAG = "WiFi";

// WiFi credentials
#define WIFI_SSID "FRITZ!Box 7530 KW"
#define WIFI_PASS "43985539165212248477"

// InfluxDB configuration
// URL
#define INFLUXDB_URL "http://192.168.0.100:8086/api/v2/write"

// Credentials
#define INFLUXDB_BUCKET "meditions"      
#define INFLUXDB_ORG "stationtest" 
#define INFLUXDB_TOKEN "TM9R0O332u8dfSW01CrSKenU2NcWKs80xY6lmn1GGWOc6Ey8TqELg5hWq3uHLoE0qh4Wr32CtLN8UVl25cyUPA=="  // Token generado por InfluxDB

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

    // Register event handlers for WiFi & IP
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    // WiFi configuration
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); // Station mode (connection to an existing network)
    ESP_ERROR_CHECK(esp_wifi_set_config((wifi_interface_t)ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start()); // WiFi init

    ESP_LOGI(TAG, "WiFi inicializado. Intentando conectar...");
}

void send_sensor_data(float temperature, float humidity, int data_count) {
    char post_data[200];
    snprintf(post_data, sizeof(post_data),
             "sensores,dispositivo=esp32-c3 temperatura=%.2f,humedad=%.2f,count=%d",
             temperature, humidity, data_count);

    char full_url[300];
    snprintf(full_url, sizeof(full_url), "%s?bucket=%s&org=%s", INFLUXDB_URL, INFLUXDB_BUCKET, INFLUXDB_ORG);

    esp_http_client_config_t config = {
        .url = full_url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Authorization", "Token " INFLUXDB_TOKEN);
    esp_http_client_set_header(client, "Content-Type", "text/plain");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Datos enviados: %s", post_data);
    } else {
        ESP_LOGE(TAG, "Error al enviar datos: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

extern "C" void app_main() {
    
    printf("¡Hola Mundo! ESP32-C3 está funcionando\n");

    // Initialization
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();

    // WiFi connection: waiting msg
    ESP_LOGI(TAG, "Esperando conexión WiFi...");
    vTaskDelay(pdMS_TO_TICKS(5000));

    ESP_LOGI(TAG, "Comenzando simulación de envío de datos a InfluxDB");

    // Proof of concept: sending 5 sensor readings
    for (int i = 1; i <= 5; i++) {
        float temperatura = 20.0 + (rand() % 100) / 10.0; // Simulación: 20.0°C a 29.9°C
        float humedad = 50.0 + (rand() % 500) / 10.0;     // Simulación: 50.0% a 99.9%
        ESP_LOGI(TAG, "Medición %d: Temperatura=%.2f, Humedad=%.2f", i, temperatura, humedad);
        send_sensor_data(temperatura, humedad, i);
        vTaskDelay(pdMS_TO_TICKS(2000)); // Pausa de 2 segundos entre envíos
    }

    ESP_LOGI(TAG, "Datos enviados a InfluxDB. Fin de la simulación.");
}