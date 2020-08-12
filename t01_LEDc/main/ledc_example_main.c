#include <stdio.h>
#include "driver/gpio.h"
#include "esp_err.h"

#define HIGH_LEVEL 1

void app_main()
{
	ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_26, HIGH_LEVEL));
	ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_27, HIGH_LEVEL));
	ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_33, HIGH_LEVEL));

	ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT));
	ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT));
	ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT));
}
