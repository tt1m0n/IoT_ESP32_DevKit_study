#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LED1 (27)
#define SW1  (39)

#define LED2 (26)
#define SW2  (18)

#define HIGH_LEVEL (1)
#define LOW_LEVEL (0)

#define BTN_UP    (1)
#define BTN_DOWN  (0)

void button_checking(int button_gpio, int led_gpio, bool* led_on, bool* btn_down)
{
    if (gpio_get_level(button_gpio) == BTN_DOWN) {
        *btn_down = true;
    }
	// press - down and up button
	// first press - led on, second press - led off, etc.
	else if (gpio_get_level(button_gpio) == BTN_UP && *btn_down) { // when button in UP position AFTER down position
        if (*led_on) {
            *led_on = false;
        } else {
            *led_on = true;
        }

        // update btn_down flag
        *btn_down = false;
    }

    if(*led_on){
        ESP_ERROR_CHECK(gpio_set_level(led_gpio, HIGH_LEVEL));
    } else {
        ESP_ERROR_CHECK(gpio_set_level(led_gpio, LOW_LEVEL));
    }
}

void app_main()
{
    // SW1 and SW2 can have only input mode
    ESP_ERROR_CHECK(gpio_set_direction(SW1, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(SW2, GPIO_MODE_INPUT));

    // set mode output for LED1 and LED2
    ESP_ERROR_CHECK(gpio_set_direction(LED1, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LED2, GPIO_MODE_OUTPUT));

    // initialize helping flags
    bool led1_on = 0;
    bool btn1_down = 0;
    bool led2_on = 0;
    bool btn2_down = 0;

    while(true) {
        button_checking(SW1, LED1, &led1_on, &btn1_down);
        button_checking(SW2, LED2, &led2_on, &btn2_down);

        // use it for avoiding watchdog errors
        vTaskDelay(10);
    }
}
