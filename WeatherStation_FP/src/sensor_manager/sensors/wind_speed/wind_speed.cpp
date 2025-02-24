#include "wind_speed.h"

WindSpeedSensor::WindSpeedSensor(adc1_channel_t channel) {
    adc = new ADC(channel);
    adc->adcSetup();
}

uint32_t WindSpeedSensor::getSpeed() {
    uint32_t raw_value = adc->readRaw();
    float voltage = (raw_value * 3.3) / adc->MAX_ADC_VALUE; // 3.3V belongs to ESP32 max voltage for ADC
    float speed_ms = (voltage / 3.3) * 30; // 0-3.3V to 0-30 m/s
    return static_cast<uint32_t>(speed_ms);
}

WindSpeedSensor::~WindSpeedSensor() {
    delete adc;
}