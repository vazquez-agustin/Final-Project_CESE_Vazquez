#include "ADC.h"

class ADC {
private:
    adc1_channel_t channel;
    esp_adc_cal_characteristics_t *adc_chars;
    adc_unit_t unit;
    adc_atten_t attenuation;

public:
    // Variables
    const int maxADCValue = 4095; // Max ADV value of soil moisture
    const int refVoltage = 1100;  // With adc2_vref_to_gpio() I could obtain a better estimate
    // Constructor
    ADC(adc1_channel_t _channel, adc_unit_t _unit = ADC_UNIT_1, adc_atten_t _attenuation = ADC_ATTEN_DB_11)
        : channel(_channel), unit(_unit), attenuation(_attenuation) {
        // ADC initialization
        adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    }

    // ADC Configuration
    void adcSetup() {
        // ADC width config
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, attenuation);
        // Characterize ADC
        esp_adc_cal_characterize(unit, attenuation, ADC_WIDTH_BIT_12, refVoltage, adc_chars);
    }

    // Read raw value
    uint32_t readRaw() {
        return adc1_get_raw((adc1_channel_t)channel);
    }

    ~ADC() {
        if (adc_chars != nullptr) {
            free(adc_chars);
        }
    }
};