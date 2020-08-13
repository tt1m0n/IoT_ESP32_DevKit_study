#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/dac.h"
#include "esp_err.h"

#define LED1 (27)
#define LED2 (26)

#define LEDC_TEST_CH_NUM (2)
#define LEDC_TEST_DUTY (4000)
#define LEDC_TEST_FADE_TIME (1000)

#define TIME_BEFORE_CHANGE_VOL (15)

void app_main()
{
    int ch;

    /*
     * Prepare and set configuration of timer
     * that will be used by LED Controller
     */
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE,   // timer mode
        .timer_num = LEDC_TIMER_0,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };
    // Set configuration of timer
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    /*
     * Prepare individual configuration
     * for channel of LED Controller by selecting:
     * - controller's channel number
     * - output duty cycle, set initially to 0
     * - GPIO number where LED is connected to
     * - speed mode, either high or low
     * - timer servicing selected channel
     *   Note: if different channels use one timer,
     *         then frequency and bit_num of these channels
     *         will be the same
     */
    ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
        {.channel = LEDC_CHANNEL_0,
         .duty = 0,
         .gpio_num = LED1,
         .speed_mode = LEDC_HIGH_SPEED_MODE,
         .hpoint = 0,
         .timer_sel = LEDC_TIMER_0},
        {.channel = LEDC_CHANNEL_1,
         .duty = 0,
         .gpio_num = LED2,
         .speed_mode = LEDC_HIGH_SPEED_MODE,
         .hpoint = 0,
         .timer_sel = LEDC_TIMER_0}};

    // Set LED Controller with previously prepared configuration
    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++)
    {
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[ch]));
    }

    // Initialize fade service.
    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    // Need enable dac before set voltage
    ESP_ERROR_CHECK(dac_output_enable(DAC_CHANNEL_2));

    while (1)
    {
        // fade up LED1 to duty for LEDC_TEST_DUTY
        ESP_ERROR_CHECK(ledc_set_fade_with_time(ledc_channel[0].speed_mode,
                                                ledc_channel[0].channel, LEDC_TEST_DUTY, LEDC_TEST_FADE_TIME));
        ESP_ERROR_CHECK(ledc_fade_start(ledc_channel[0].speed_mode,
                                        ledc_channel[0].channel, LEDC_FADE_NO_WAIT));

        // increase DAC output voltage for LED2 to simulate pulsing. DAC output is 8-bit. Maximum (255) corresponds to VDD3P3_RTC.
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 130));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 140));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 150));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 160));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 170));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 180));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 190));
        vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);

        // fade down LED1 to duty 0
        ESP_ERROR_CHECK(ledc_set_fade_with_time(ledc_channel[0].speed_mode,
                                                ledc_channel[0].channel, 0, LEDC_TEST_FADE_TIME));
        ESP_ERROR_CHECK(ledc_fade_start(ledc_channel[0].speed_mode,
                                        ledc_channel[0].channel, LEDC_FADE_NO_WAIT));

        // decrease DAC output voltage for LED2 to simulate pulsing. DAC output is 8-bit. Maximum (255) corresponds to VDD3P3_RTC.
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 180));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 170));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 160));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 150));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 140));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 130));
        vTaskDelay(TIME_BEFORE_CHANGE_VOL);
        ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_2, 0));
        vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);
    }

    // disable dac
    ESP_ERROR_CHECK(dac_output_disable(DAC_CHANNEL_2));
}
