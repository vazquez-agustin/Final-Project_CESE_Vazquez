#include "influxdb.h"

void InfluxDBClient::send_data(uint32_t moisture_value) {
    // Here you build the string with the data to send to InfluxDB
    std::string post_data = "humedad,location=office value=" + std::to_string(moisture_value);

    // InfluxDB URL
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
        ESP_LOGI("InfluxDB", "Datos enviados correctamente a InfluxDB. Estado: %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE("InfluxDB", "Error enviando datos: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

esp_err_t InfluxDBClient::http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI("InfluxDB", "Datos recibidos: %.*s", evt->data_len, (char *)evt->data);
            break;
        default:
            break;
    }
    return ESP_OK;
}
