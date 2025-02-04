#include "ADC.h"


ADC::ADC(adc1_channel_t _channel, adc_unit_t _unit, adc_atten_t _attenuation)
    : channel(_channel), unit(_unit), attenuation(_attenuation) {
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
}

ADC::~ADC() {
    if (adc_chars != nullptr) {
        free(adc_chars);
    }
}

void ADC::adcSetup() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, attenuation);
    esp_adc_cal_characterize(unit, attenuation, ADC_WIDTH_BIT_12, refVoltage, adc_chars);
}

uint32_t ADC::readRaw() {
    return adc1_get_raw((adc1_channel_t)channel);
}