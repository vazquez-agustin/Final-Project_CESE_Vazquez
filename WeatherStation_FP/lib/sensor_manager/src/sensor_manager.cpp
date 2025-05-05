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

/** @file  sensor_manager.cpp
 ** @brief Implementación de la clase SensorManager
 **/

/* === Headers files inclusions ================================================================ */
#include "sensor_manager.h"
/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Constructor por defecto de SensorManager.
 */
SensorManager::SensorManager()
    : soilMoistureSensor(ADC1_CHANNEL_0), windSpeedSensor(ADC1_CHANNEL_1) {
        
    }

/**
 * @brief Inicializa y establece la conexión Wi-Fi con el punto de acceso especificado.
 */
void SensorManager::initWiFi()
{

    WiFi.initialization(WIFI_SSID, WIFI_PASS);
    ESP_LOGI("WiFi", "Esperando conexión WiFi...");

    while (!WiFi.isConnected())
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    ESP_LOGI("WiFi", "Conexión WiFi establecida.");
}

/**
 * @brief Ejecuta el ciclo continuo de lecturas de sensores y transmisión a InfluxDB.
 *
 * @note Esta función corre en un bucle infinito; debería ejecutarse como una tarea separada.
 */
void SensorManager::sensorsRun()
{
    while (true)
    {
        uint32_t moisture_percentage = soilMoistureSensor.readPercentage();
        uint32_t speed_velocity = windSpeedSensor.getSpeed();
        measure_t data;

        bme680.getMeasure(&data);;

        ESP_LOGI("SensorManager", "Soil moisture: %lu%%", (unsigned long)moisture_percentage);
        ESP_LOGI("SensorManager", "Wind speed: %lu m/s", (unsigned long)speed_velocity);
        ESP_LOGI("SensorManager", "Temperature: %f °C", data.Temperature);
        ESP_LOGI("SensorManager", "Pressure: %f hPa", data.Pressure);
        ESP_LOGI("SensorManager", "Humidity: %f %%", data.Humidity);

        influxClient.send_data(moisture_percentage, speed_velocity, data.Temperature, data.Pressure, data.Humidity);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}