#ifndef RECOGNITION_H
#define RECOGNITION_H
#include "stdint.h"
#include "mbed.h"

#define SET_SIZE 10
#define THRESHOLD 500000

uint32_t dtw_cost(int16_t (&set_one)[SET_SIZE], int16_t (&set_two)[SET_SIZE]);

#endif