#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ADC pin configuration
#define SOIL_SENSOR_PIN ADC1_CHANNEL_0

static const char* TAG = "SoilMoisture";

#include "esp_adc_cal.h"


static adc1_channel_t channel;
static esp_adc_cal_characteristics_t *adc_chars;

static const adc_unit_t unit = ADC_UNIT_1;
static const adc_atten_t attenuation = ADC_ATTEN_DB_11;


void adc_setup(adc1_channel_t _channel) {
    channel = _channel;
    //Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, attenuation);
    //Characterize ADC
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, attenuation, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
}

uint32_t soilMoisture_read() {
    uint32_t adc_reading = 0;
    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_reading += adc1_get_raw((adc1_channel_t)channel);
    }
    adc_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    // uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    return adc_reading;
}

uint32_t dataNormalization(uint32_t value_t) {
    return (value_t * 100) / VALUE_MAX;
}

/*
void init_adc() {
    // Configure ADC width and channel
    adc1_config_width(ADC_WIDTH_BIT_12); // Set ADC resolution to 12-bit
    adc1_config_channel_atten(SOIL_SENSOR_PIN, ADC_ATTEN_DB_0); // Configure attenuation for 3.3V
}
*/
/*
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
*/
extern "C" void app_main() {
    adc_setup(ADC1_CHANNEL_0);
    char soilMoisture_buffer[1024];

    while(1) {
        uint32_t adc_reading = soilMoisture_read();
        uint32_t adc_percentage = dataNormalization(adc_reading);
        // Imprimir el valor del ADC y el porcentaje de humedad del suelo
        ESP_LOGI(TAG, "Soil moisture level: %lu%%", (unsigned long)adc_percentage);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}