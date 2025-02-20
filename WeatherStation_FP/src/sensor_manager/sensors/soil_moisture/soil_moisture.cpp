#include "soil_moisture.h"
#include "ADC.h"

SoilMoistureSensor::SoilMoistureSensor(adc1_channel_t channel) {
    adc = new ADC(channel);
    adc->adcSetup();
}

uint32_t SoilMoistureSensor::readPercentage() {
    uint32_t raw_value = adc->readRaw();
    return (raw_value * 100) / adc->MAX_ADC_VALUE;
}

SoilMoistureSensor::~SoilMoistureSensor() {
    delete adc;
}