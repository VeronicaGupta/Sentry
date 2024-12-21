#ifndef RECOGNITION_H
#define RECOGNITION_H
#include "stdint.h"
#include "mbed.h"

#define SET_SIZE 30
#define THRESHOLD 500000

uint32_t dtw_cost(int32_t *set_one, int32_t *set_two);

#endif