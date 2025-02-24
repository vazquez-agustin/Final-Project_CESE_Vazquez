#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

#include "ADC.h"
#include <stdint.h>
#include "driver/adc.h"

class SoilMoistureSensor {
private:
    ADC *adc;

public:
    SoilMoistureSensor(adc1_channel_t channel);
    uint32_t readPercentage();
    ~SoilMoistureSensor();
};

#endif // SOIL_MOISTURE_H

