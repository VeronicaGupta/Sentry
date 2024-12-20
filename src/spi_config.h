#ifndef SPI_H
#define SPI_H

#include "mbed.h"


#define SPI_FLAG 1
#define DATA_READY_FLAG 2


SPI spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel);
uint8_t write_buf[32], read_buf[32];

InterruptIn int2(PA_2, PullDown);
InterruptIn button(BUTTON1);
DigitalOut start_record_led_indicator(LED1);
DigitalOut stop_record_led_indicator(LED2);

// Define Flag bits for the EventFlags object
EventFlags flags;

const char* filename = "/fs/gyro_data.json";
const char* filename_test = "/fs/gyro_data_test.json";

#endif