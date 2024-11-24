
#include "main.h"
#include "driver/gpio.h"

#define HIGH 1
#define LOW  0

void app_main() {

    printf("Hello, World!\n");

    // LED is configured as an output
    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while(1)
    {
        gpio_set_level(LED_PIN, HIGH);   // Turn LED on
        printf("LED On.\n");
        esp_rom_delay_us(500000);     // 500,000 us = 500 ms

        gpio_set_level(LED_PIN, LOW);   // Turn LED off
        printf("LED Off.\n");
        esp_rom_delay_us(500000);
    }
}