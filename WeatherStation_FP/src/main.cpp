#include "main.h"
#include "soil_moisture.h"
#include "WiFi.h"
#include "influxdb.h"

extern "C" void app_main() {
    printf("¡Hola Mundo! ESP32-C3 está funcionando\n");

    ESP_ERROR_CHECK(nvs_flash_init());
    WiFiManager wifi;
    wifi.initialization();

    ESP_LOGI("WiFi", "Esperando conexión WiFi...");
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Creates a task that will read the soil moisture sensor and send the data to InfluxDB
    adc1_channel_t soil_sensor_channel = ADC1_CHANNEL_0;
    SoilMoistureSensor soilSensor(soil_sensor_channel);

    // Creates an InfluxDB client
    InfluxDBClient influxClient;

    while (true) {
        // Reads the soil moisture sensor
        uint32_t moisture_percentage = soilSensor.readPercentage();

        ESP_LOGI("Soil Moisture", "Soil moisture level: %lu%%", (unsigned long)moisture_percentage);

        influxClient.send_data(moisture_percentage);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
