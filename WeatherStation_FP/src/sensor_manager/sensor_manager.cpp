#include "sensor_manager.h"

SensorManager::SensorManager() 
    : soilMoistureSensor(ADC1_CHANNEL_0), windSpeedSensor(ADC1_CHANNEL_1) {}

void SensorManager::sensorsRun() {
    while (true) {
        // Read sensor data
        uint32_t moisture_percentage = soilMoistureSensor.readPercentage();
        uint32_t speed_velocity = windSpeedSensor.getSpeed();

        ESP_LOGI("SensorManager", "Soil moisture: %lu%%", (unsigned long)moisture_percentage);
        ESP_LOGI("SensorManager", "Wind speed: %lu m/s", (unsigned long)speed_velocity);

        influxClient.send_data(moisture_percentage, speed_velocity);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
