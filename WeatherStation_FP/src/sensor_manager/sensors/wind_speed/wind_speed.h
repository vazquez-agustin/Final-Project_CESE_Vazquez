#ifndef WIND_SPEED_H
#define WIND_SPEED_H

#include "ADC.h"
#include <iostream>
#include <stdint.h>
#include "driver/adc.h"

class WindSpeedSensor {
private:
    ADC *adc;

public:
    WindSpeedSensor(adc1_channel_t channel);
    uint32_t getSpeed();
    ~WindSpeedSensor();
};

#endif // WIND_SPEED_H