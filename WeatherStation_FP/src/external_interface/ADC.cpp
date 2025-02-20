#include "ADC.h"

ADC::ADC(adc1_channel_t _channel, adc_unit_t _unit, adc_atten_t _attenuation, esp_adc_cal_characteristics_t *_adc_chars)
    : channel(_channel), unit(_unit), attenuation(_attenuation), adc_chars(nullptr) {
    }

ADC::~ADC() {
    if (adc_chars) {
        free(adc_chars);
        adc_chars = nullptr;
    }
}

void ADC::adcSetup() {

    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, attenuation);
    esp_adc_cal_characterize(unit, attenuation, ADC_WIDTH_BIT_12, REF_VOLTAGE, adc_chars);
}

uint32_t ADC::readRaw() {
    return adc1_get_raw((adc1_channel_t)channel);
}
