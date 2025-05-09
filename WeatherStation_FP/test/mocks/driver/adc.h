#ifndef DRIVER_ADC_H
#define DRIVER_ADC_H

#include "../esp_err.h"

typedef enum {
    ADC_ATTEN_DB_0   = 0,  /*!< No input attenuation, ADC can measure up to approx. 800 mV */
    ADC_ATTEN_DB_2_5 = 1,  /*!< The input voltage of ADC will be attenuated, extending the range of measurement to approx. 1100 mV */
    ADC_ATTEN_DB_6   = 2,  /*!< The input voltage of ADC will be attenuated, extending the range of measurement to approx. 1350 mV */
    ADC_ATTEN_DB_11  = 3,  /*!< The input voltage of ADC will be attenuated, extending the range of measurement to approx. 2600 mV */
} adc_atten_t;

typedef enum {
    ADC_WIDTH_BIT_9  = 0,  /*!< ADC capture width is 9Bit. */
    ADC_WIDTH_BIT_10 = 1,  /*!< ADC capture width is 10Bit. */
    ADC_WIDTH_BIT_11 = 2,  /*!< ADC capture width is 11Bit. */
    ADC_WIDTH_BIT_12 = 3,  /*!< ADC capture width is 12Bit. */
} adc_bits_width_t;

typedef enum {
    ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO36 */
    ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO37 */
    ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO38 */
    ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO39 */
    ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO32 */
    ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO33 */
    ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO34 */
    ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO35 */
    ADC1_CHANNEL_MAX,
} adc1_channel_t;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t adc1_config_width(adc_bits_width_t width_bit);
esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten);
int adc1_get_raw(adc1_channel_t channel);

#ifdef __cplusplus
}
#endif

#endif // DRIVER_ADC_H