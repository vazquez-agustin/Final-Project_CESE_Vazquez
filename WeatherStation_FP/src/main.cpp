#include "main.h"
#include "soil_moisture.h"

static const char* TAG = "SoilMoisture";

extern "C" void app_main() {
    // Definir el canal ADC donde está conectado el sensor de humedad del suelo
    adc1_channel_t soil_sensor_channel = ADC1_CHANNEL_0;
    
    // Crear una instancia del sensor de humedad del suelo
    SoilMoistureSensor soilSensor(soil_sensor_channel);

    while (true) {
        // Leer el nivel de humedad en porcentaje
        uint32_t moisture_percentage = soilSensor.readPercentage();
        
        // Imprimir el valor en el log
        ESP_LOGI(TAG, "Soil moisture level: %lu%%", (unsigned long)moisture_percentage);
        
        // Esperar 1 segundo antes de la siguiente lectura
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
