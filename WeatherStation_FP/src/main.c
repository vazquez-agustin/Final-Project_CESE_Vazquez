
#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define HIGH 1
#define LOW  0

void app_main() {

    printf("Hello, World!\n");

    // LED is configured as an output
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while(1)
    {
        gpio_set_level(LED_PIN, HIGH);   // Turn LED on
        printf("LED On.\n");
        vTaskDelay(pdMS_TO_TICKS(500));     // 500,000 us = 500 ms

        gpio_set_level(LED_PIN, LOW);   // Turn LED off
        printf("LED Off.\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}