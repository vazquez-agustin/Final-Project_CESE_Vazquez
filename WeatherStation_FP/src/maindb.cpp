#include "main.h"

static const char *TAG = "WiFi";

// WiFi credentials
#define WIFI_SSID "FRITZ!Box 7530 KW"
#define WIFI_PASS "43985539165212248477"

// InfluxDB configuration
// URL
#define INFLUXDB_IP "192.168.178.129"
// Credentials
#define INFLUXDB_BUCKET "meditions"      
#define INFLUXDB_ORG "stationtest" 
#define INFLUXDB_TOKEN "rwSRkEyDRFF_Y6JMqD1JxZ_lZUJzFCGUIgzj9Li9nYlo7XUtQPXbCxY1Xvs3i-4z3-cbY7FwEp2hoI6r1P8LQQ=="  // Token generado por InfluxDB

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

// HTTP event handler (optional, logs responses)
esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "Datos recibidos: %.*s", evt->data_len, (char *)evt->data);
            break;
        default:
            break;
    }
    return ESP_OK;
}

// Function to send data to InfluxDB
void send_data_to_influxdb() {
    const char *post_data = "humedad,location=office value=25.5"; // Line Protocol

    esp_http_client_config_t config = {
        .url = "http://192.168.178.129:8086/api/v2/write?org=stationtest&bucket=measurements&precision=s",
        .method = HTTP_METHOD_POST,
        .event_handler = http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Authorization", "Token " INFLUXDB_TOKEN);
    esp_http_client_set_header(client, "Content-Type", "text/plain");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Datos enviados correctamente a InfluxDB. Estado: %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "Error enviando datos: %s", esp_err_to_name(err));
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

    send_data_to_influxdb();
    // Periodic task to send data
    //while (true) {
    //    send_data_to_influxdb();
    //    vTaskDelay(pdMS_TO_TICKS(10000)); // Envia cada 10 segundos
    //}

}