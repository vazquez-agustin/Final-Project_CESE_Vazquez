#include "main.h"
#include "soil_moisture.h"
#include "WiFi.h"
#include "influxdb.h"

static const char* TAG = "SoilMoisture";

extern "C" void app_main() {
    printf("¡Hola Mundo! ESP32-C3 está funcionando\n");

    // Inicialización de WiFi
    ESP_ERROR_CHECK(nvs_flash_init());
    WiFiManager wifi;
    wifi.start();

    // Esperar la conexión WiFi
    ESP_LOGI(TAG, "Esperando conexión WiFi...");
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Crear la instancia del sensor de humedad
    adc1_channel_t soil_sensor_channel = ADC1_CHANNEL_0;
    SoilMoistureSensor soilSensor(soil_sensor_channel);

    // Crear la instancia del cliente de InfluxDB
    InfluxDBClient influxClient;

    while (true) {
        // Leer la humedad del suelo
        uint32_t moisture_percentage = soilSensor.readPercentage();

        // Imprimir el valor en el log
        ESP_LOGI(TAG, "Soil moisture level: %lu%%", (unsigned long)moisture_percentage);

        // Enviar la medición a InfluxDB
        influxClient.send_data(moisture_percentage);

        // Esperar 5 segundos antes de la siguiente lectura y envío
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
