#ifndef UART_COMMUNICATION_H_
#define UART_COMMUNICATION_H_

#include "driver/uart.h"

void UART_init(int uart_num, int baud_rate, int buf_size);

#endif