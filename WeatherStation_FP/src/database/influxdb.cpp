#include "influxdb.h"
#include <string>

#define INFLUXDB_URL "http://192.168.0.100:8086/api/v2/write"
#define INFLUXDB_TOKEN "vND8qiFkSJqwD0kZqPAGilY16z3fTexXVbOlYb968a6wFsGj9MT5JCkcI3aforCHT9ncyz33RE3gYOKx20Rccw=="

static const char* TAG = "InfluxDB";

void InfluxDBClient::send_data(uint32_t moisture_value) {
    // Construcción de la cadena en Line Protocol con el valor de humedad
    std::string post_data = "humedad,location=office value=" + std::to_string(moisture_value);

    // Construcción de la URL
    std::string url = "http://192.168.178.129:8086/api/v2/write?org=stationtest&bucket=measurements&precision=s";

    esp_http_client_config_t config = {};
    config.url = url.c_str();
    config.method = HTTP_METHOD_POST;
    config.event_handler = http_event_handler;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Authorization", "Token " INFLUXDB_TOKEN);
    esp_http_client_set_header(client, "Content-Type", "text/plain");
    esp_http_client_set_post_field(client, post_data.c_str(), post_data.length());

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Datos enviados correctamente a InfluxDB. Estado: %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "Error enviando datos: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

esp_err_t InfluxDBClient::http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "Datos recibidos: %.*s", evt->data_len, (char *)evt->data);
            break;
        default:
            break;
    }
    return ESP_OK;
}
