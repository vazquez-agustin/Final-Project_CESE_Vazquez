#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc_cal.h"

static const char* TAG = "SoilMoisture";

class ADC {
private:
    adc1_channel_t channel;
    esp_adc_cal_characteristics_t *adc_chars;
    adc_unit_t unit;
    adc_atten_t attenuation;

public:
    // Constructor
    ADC(adc1_channel_t _channel, adc_unit_t _unit = ADC_UNIT_1, adc_atten_t _attenuation = ADC_ATTEN_DB_11)
        : channel(_channel), unit(_unit), attenuation(_attenuation) {
        // ADC initialization
        adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    }

    // ADC Configuration
    void setup() {
        // ADC width config
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, attenuation);
        // Characterize ADC
        esp_adc_cal_characterize(unit, attenuation, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    }

    // Read raw value
    uint32_t readRaw() {
        uint32_t adc_reading = 0;
        // Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading += adc1_get_raw(channel);
        }
        return adc_reading / NO_OF_SAMPLES;
    }

    // Read voltage (mV)
    uint32_t readVoltage() {
        uint32_t raw = readRaw();
        return esp_adc_cal_raw_to_voltage(raw, adc_chars);
    }

    ~ADC() {
        if (adc_chars != nullptr) {
            free(adc_chars);
        }
    }
};

class SoilMoistureSensor {
private:
    ADC *adc;

public:
    // Constructor
    SoilMoistureSensor(adc1_channel_t channel) {
        adc = new ADC(channel);
        adc->setup();
    }

    // Read the moisture level (raw ADC value)
    uint32_t readMoistureRaw() {
        return adc->readRaw();
    }

    // Normalize the moisture level to a percentage (0-100%)
    uint32_t readMoisturePercentage() {
        uint32_t raw_value = readMoistureRaw();
        return (raw_value * 100) / VALUE_MAX;
    }

    ~SoilMoistureSensor() {
        delete adc;
    }
};

extern "C" void app_main() {

    adc1_channel_t soil_sensor_channel = ADC1_CHANNEL_0;
    SoilMoistureSensor soilSensor(soil_sensor_channel);

    while (true) {
        // Read the moisture level
        uint32_t moisture_percentage = soilSensor.readMoisturePercentage();
        ESP_LOGI(TAG, "Soil moisture level: %lu%%", (unsigned long)moisture_percentage);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 1s
    }

}