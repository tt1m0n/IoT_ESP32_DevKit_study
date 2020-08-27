#include "esp_timer.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht11.h"

#define LOW_LEVEL 0
#define HIGH_LEVEL 1

#define DHT_GPIO                     GPIO_NUM_4
#define DHT_POWER_MANAGE_GPIO        GPIO_NUM_2
#define BYTES_RESPONSE               5
#define BYTE_SIZE                    8
#define PAUSE_BEFORE_SEND_ONE_BIT_US 50
#define TIME_FOR_SEND_DATA_1_US      70
#define TIME_FOR_SEND_DATA_0_US      28

#define SAMPLING_PERIOD_US    2000000

static int64_t last_read_time = -(SAMPLING_PERIOD_US);
static struct dht11_reading last_read;

static int wait_us_or_timeout(uint16_t microSeconds, int level) {
	int micros_ticks = 0;
    while(gpio_get_level(DHT_GPIO) == level) {
        ets_delay_us(1);
        if(++micros_ticks > microSeconds)
            return DHT11_TIMEOUT_ERROR;
    }

    return micros_ticks;
}

static int check_checksum(uint8_t data[]) {
    if(data[4] == (data[0] + data[1] + data[2] + data[3]))
        return DHT11_OK;
    else
        return DHT11_CHECKSUM_ERROR;
}

static void send_start_signal() {
    gpio_set_direction(DHT_GPIO, GPIO_MODE_OUTPUT);

    // set LOW LEVEL and wait at least 18ms to ensure DHT’s detection of MCU's signal
    gpio_set_level(DHT_GPIO, LOW_LEVEL);
    vTaskDelay(20 / portTICK_PERIOD_MS);

    // pull up voltage to HIGH LEVEL and wait 20-40us for DHT’s response
    gpio_set_level(DHT_GPIO, HIGH_LEVEL);
    ets_delay_us(40);

    gpio_set_direction(DHT_GPIO, GPIO_MODE_INPUT);
}

static int check_response() {
    // Once DHT detects the start signal, it will send out a
    // low-voltage-level response signal, which lasts 80us
    if(wait_us_or_timeout(80, LOW_LEVEL) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    // Then DHT sets voltage level from low to high and
    // keeps it for 80us for DHT’s preparation for sending data.
    if(wait_us_or_timeout(80, HIGH_LEVEL) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    return DHT11_OK;
}

static struct dht11_reading timeout_error() {
    struct dht11_reading timeoutError = {DHT11_TIMEOUT_ERROR, -1, -1};
    return timeoutError;
}

static struct dht11_reading checksum_error() {
    struct dht11_reading crcError = {DHT11_CHECKSUM_ERROR, -1, -1};
    return crcError;
}

void DHT11_init() {
    // power dht
    gpio_set_direction(DHT_POWER_MANAGE_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_POWER_MANAGE_GPIO, HIGH_LEVEL);

    // Wait 2 seconds to make the device pass its initial unstable status
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}

struct dht11_reading DHT11_read() {
    // Tried to sense too soon since last read (dht11 needs ~2 seconds to make a new read) */
    if(esp_timer_get_time() - SAMPLING_PERIOD_US < last_read_time) {
        return last_read;
    }

    last_read_time = esp_timer_get_time();

    send_start_signal();

    if(check_response() == DHT11_TIMEOUT_ERROR)
        return timeout_error();

    // Read response
    uint8_t data[BYTES_RESPONSE] = {0,0,0,0,0};
    for(int i = 0; i < BYTES_RESPONSE * BYTE_SIZE; i++) {
        // Initial data
        if(wait_us_or_timeout(PAUSE_BEFORE_SEND_ONE_BIT_US, LOW_LEVEL) == DHT11_TIMEOUT_ERROR)
            return timeout_error();

        if(wait_us_or_timeout(TIME_FOR_SEND_DATA_1_US, HIGH_LEVEL) > TIME_FOR_SEND_DATA_0_US) {
            // Bit received was a 1
            // do not forget that sensor sends higher data bit first
            data[i / BYTE_SIZE] |= (1 << (7 - (i % BYTE_SIZE)));
        }
    }

    if(check_checksum(data) != DHT11_CHECKSUM_ERROR) {
        last_read.status = DHT11_OK;
        last_read.temperature = data[2];
        last_read.humidity = data[0];
        return last_read;
    } else {
        return checksum_error();
    }
}