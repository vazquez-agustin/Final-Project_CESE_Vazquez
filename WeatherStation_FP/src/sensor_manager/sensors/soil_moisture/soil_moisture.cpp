#include "soil_moisture.h"
#include "ADC.h"

static const char* TAG = "SoilMoisture";


SoilMoistureSensor::SoilMoistureSensor(adc1_channel_t channel) {
    adc = new ADC(channel);
    adc->adcSetup();
}

uint32_t SoilMoistureSensor::readPercentage() {
    uint32_t raw_value = adc->readRaw();
    return (raw_value * 100) / adc->maxADCValue;
}

SoilMoistureSensor::~SoilMoistureSensor() {
    delete adc;
}