#include "sensor_manager.h"

SensorManager::SensorManager() 
    : soilSensor(ADC1_CHANNEL_0), windSensor() {}

void SensorManager::sensorsRun() {
    while (true) {
        // Read sensor data
        uint32_t moisture_percentage = soilSensor.readPercentage();
        windSensor.readSpeed();

        ESP_LOGI("SensorManager", "Soil moisture: %lu%%", (unsigned long)moisture_percentage);
        // LOG_INFO("Wind speed: %f m/s", windSensor.getSpeed());

        influxClient.send_data(moisture_percentage);
        // influxClient.send_data(windSensor.getSpeed());

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
