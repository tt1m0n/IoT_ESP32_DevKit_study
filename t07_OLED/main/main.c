#include "driver/gpio.h"
#include "driver/i2c.h"

#include "freertos/task.h"
#include <stdio.h>
#include "sh1106.h"

#define SDA_PIN  GPIO_NUM_21
#define SCL_PIN  GPIO_NUM_22
#define I2C_PORT SH1106_DEFAULT_PORT

/*
    Initialize I2C driver.
*/
void init_i2c() {
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 1000000
    };
    i2c_param_config(I2C_PORT, &i2c_config);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
}

void app_main() {
	gpio_set_level(GPIO_NUM_32, 1);
    gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
	vTaskDelay(2000 / portTICK_PERIOD_MS);

	printf("TEST0\n");
    init_i2c();
	printf("TEST1\n");


    // setup sh1106 display
    sh1106_t display;
    display.addr = SH1106_DEFAULT_ADDR;
    display.port = I2C_PORT;
    printf("TEST2\n");
    sh1106_init(&display);
    printf("TEST3\n");
    //draw triangle
    for (uint8_t y = 0; y < 128; y++) {
        for (uint8_t x = 0; x < 128; x++) {
            sh1106_set(&display, x, y, (x & y) == 0);
        }
    }
    printf("TEST4\n");

    // update display
    sh1106_update(&display);
	printf("TEST5\n");
}
