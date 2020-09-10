#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/dac.h"
#include "esp_err.h"

void make_sound(int number_of_waves,
    int delay_btw_wave_cycle,
    int max_voltage,
    int frequency,                // increase voltage from 0 to max_voltage by this value
    int delay_btw_voltage_output)
{
    for (int cycle = 0; cycle < number_of_waves; ++cycle) {
        for (int voltage = 0; voltage < max_voltage; voltage += frequency) {
            ESP_ERROR_CHECK(dac_output_voltage(DAC_CHANNEL_1, voltage));
            ets_delay_us(delay_btw_voltage_output);
        }
        ets_delay_us(delay_btw_wave_cycle);
    }
}

void app_main()
{
    dac_output_enable(DAC_CHANNEL_1);
    make_sound(500, 10, 255, 2, 2);
    // disable dac
    ESP_ERROR_CHECK(dac_output_disable(DAC_CHANNEL_1));
}
