#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "esp_err.h"

#define DEFAULT_VREF 1100 //With adc2_vref_to_gpio() I could obtain a better estimate
#define NO_OF_SAMPLES 64 //Multisampling

#define VALUE_MAX 4095 // Max ADV value of soil mosture
