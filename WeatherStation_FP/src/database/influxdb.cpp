/************************************************************************************************
 * 
 *  @author     Agustín Jesús Vazquez <vazqueza193@gmail.com>
 *  @date       Marzo, 2025
 *  @version    1.0
 *
 *  @license    MIT License
 * 
Copyright (c) 2025, Agustín Jesús Vazquez <vazqueza193@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

/** @file  influxdb.cpp
 ** @brief Implementación de la clase InfluxDBClient.
 **/

/* === Headers files inclusions ================================================================ */
#include "influxdb.h"
/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Envía datos de sensores hacia la base de datos InfluxDB.
 * 
 * @param moisture_value Valor numérico de la humedad del suelo (0-100%).
 * @param speed_value Valor numérico de la velocidad del viento en m/s.
 */
void InfluxDBClient::send_data(uint32_t moisture_value, uint32_t speed_value, float temperature_value, float pressure_value, float humidity_value) {
    // String with the data to send to InfluxDB
    std::string post_data = "humedad_de_suelo,location=outside value=" + std::to_string(moisture_value) + "\n" +
                            "velocidad_de_viento,location=outside value=" + std::to_string(speed_value) + "\n" +
                            "temperatura,location=outside value=" + std::to_string(temperature_value) + "\n" +
                            "humedad,location=outside value=" + std::to_string(humidity_value) + "\n" +
                            "presion,location=outside value=" + std::to_string(pressure_value);

    std::string url = INFLUXDB_URL;

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

/**
 * @brief Manejador de eventos HTTP para el cliente de InfluxDB.
 * 
 * @param evt Puntero a la estructura que contiene información del evento HTTP.
 * 
 * @return Código de error ESP_OK tras procesar el evento.
 */
esp_err_t InfluxDBClient::http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGE("InfluxDB", "HTTP Event Error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI("InfluxDB", "Connected to InfluxDB");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI("InfluxDB", "Headers sent");
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI("InfluxDB", "Data received: %.*s", evt->data_len, (char *)evt->data);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI("InfluxDB", "HTTP request finished");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI("InfluxDB", "Disconnected from InfluxDB");
            break;
        default:
            break;
    }
    return ESP_OK;
}