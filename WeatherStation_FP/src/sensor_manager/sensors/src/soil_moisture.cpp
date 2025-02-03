#include "soil_moisture.h"
#include "ADC.h"

static const char* TAG = "SoilMoisture";

class SoilMoistureSensor {
private:
    ADC *adc;

public:
    // Constructor
    SoilMoistureSensor(adc1_channel_t channel) {
        adc = new ADC(channel);
        adc->adcSetup();
    }

    // Read the moisture level (raw ADC value) and it normalizes the value to a percentage (0-100%)
    uint32_t readPercentage() {
        uint32_t raw_value = adc->readRaw();
        return (raw_value * 100) / adc->maxADCValue;
    }

    ~SoilMoistureSensor() {
        delete adc;
    }
};