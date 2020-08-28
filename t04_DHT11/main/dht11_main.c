#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "dht11.h"

void app_main()
{
    DHT11_init();

    while(1) {
        printf("Temperature: %d C\n", DHT11_read().temperature);
        printf("Humidity: %d %%\n", DHT11_read().humidity);
        printf("Status: %d \n", DHT11_read().status);

        vTaskDelay(1000);
    }
}