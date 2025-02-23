#ifndef ADC_H
#define ADC_H

#include "driver/adc.h"
#include "esp_adc_cal.h"

class ADC {
private:
    adc1_channel_t channel;
    adc_unit_t unit;
    adc_atten_t attenuation;
    esp_adc_cal_characteristics_t *adc_chars;

public:
    const int MAX_ADC_VALUE = 4095;
    const int REF_VOLTAGE = 1100;


    ADC(adc1_channel_t _channel, adc_unit_t _unit = ADC_UNIT_1, adc_atten_t _attenuation = ADC_ATTEN_DB_12, esp_adc_cal_characteristics_t *_adc_chars = nullptr);
    ~ADC();
    
    void adcSetup();
    uint32_t readRaw();
};

#endif // ADC_H
