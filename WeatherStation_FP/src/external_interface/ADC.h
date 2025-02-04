#ifndef ADC_H
#define ADC_H

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc_cal.h"

class ADC {
private:
    adc1_channel_t channel;
    esp_adc_cal_characteristics_t *adc_chars;
    adc_unit_t unit;
    adc_atten_t attenuation;

public:
    const int maxADCValue = 4095;
    const int refVoltage = 1100;

    ADC(adc1_channel_t _channel, adc_unit_t _unit = ADC_UNIT_1, adc_atten_t _attenuation = ADC_ATTEN_DB_11);
    ~ADC();
    
    void adcSetup();
    uint32_t readRaw();
};

#endif // ADC_H