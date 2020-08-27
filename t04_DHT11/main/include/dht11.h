#ifndef DHT11_H_
#define DHT11_H_

#include "driver/gpio.h"

enum dht11_status {
    DHT11_CHECKSUM_ERROR = -2,
    DHT11_TIMEOUT_ERROR,
    DHT11_OK
};

struct dht11_reading {
    int status;
    int temperature;
    int humidity;
};

void DHT11_init();
struct dht11_reading DHT11_read();

#endif