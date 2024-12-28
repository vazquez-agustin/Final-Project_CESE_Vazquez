#include "main.h"
#include <portmacro.h>
#include <freertos/mpu_wrappers.h>
#include <inttypes.h>

// ADC pin configuration
#define SOIL_SENSOR_PIN ADC1_CHANNEL_0

static const char* TAG = "SoilMoisture";

void init_adc() {
    // Configure ADC width and channel
    adc1_config_width(ADC_WIDTH_BIT_12); // Set ADC resolution to 12-bit
    adc1_config_channel_atten(SOIL_SENSOR_PIN, ADC_ATTEN_DB_0); // Configure attenuation for 3.3V
}

uint32_t read_soil_moisture() {
    // Read the ADC value
    int adc_value = adc1_get_raw(SOIL_SENSOR_PIN);
    if (adc_value < 0) {
        ESP_LOGE(TAG, "Failed to read ADC value");
        return 0;
    }
    ESP_LOGI(TAG, "ADC Value: %d", adc_value);
    return (uint32_t)adc_value;
}

extern "C" void app_main() {
    init_adc();
    
    uint32_t moisture_level = read_soil_moisture();
    ESP_LOGI(TAG, "Soil moisture level: %" PRIu32, moisture_level);
}