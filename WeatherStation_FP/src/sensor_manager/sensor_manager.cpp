#include "sensor_manager.h"

SensorManager::SensorManager() 
    : soilSensor(ADC1_CHANNEL_0), windSensor() {}

void SensorManager::sensorsRun() {
    while (true) {
        // Leer sensores
        uint32_t moisture_percentage = soilSensor.readPercentage();
        windSensor.readSpeed();

        // Log de datos
        ESP_LOGI("SensorManager", "Soil moisture: %lu%%", (unsigned long)moisture_percentage);

        // Enviar datos a InfluxDB
        influxClient.send_data(moisture_percentage);

        // Esperar antes de la siguiente medición
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
