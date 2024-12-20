#ifndef SPI_H
#define SPI_H

#include "mbed.h"


#define SPI_FLAG 1
#define DATA_READY_FLAG 2


SPI spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel);
uint8_t write_buf[32], read_buf[32];

// Define Flag bits for the EventFlags object
EventFlags flags;


#endif