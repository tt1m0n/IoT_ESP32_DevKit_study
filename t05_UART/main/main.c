#include <string.h>
#include "uart_communication.h"

#define UART_BAUD_RATE (115200)
#define BUF_SIZE       (1024)

void app_main(void)
{
    UART_init(UART_NUM_2, UART_BAUD_RATE, BUF_SIZE);

    char* rgb_str = "\x1b[48;2;41mRED\x1b[48;2;0m "   // set red color and then reset
                    "\x1b[48;2;42mGREEN\x1b[48;2;0m " // set green color and then reset
                    "\x1b[48;2;44mBLUE\x1b[48;2;0m "  // set blue color and then reset
                    "DEFAULT\n";

    uart_write_bytes(UART_NUM_2, (const char *)rgb_str, strlen(rgb_str));
}