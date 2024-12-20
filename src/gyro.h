#ifndef GYRO_H
#define GYRO_H

#include "stdint.h"

// Window Size for Moving Average Window
#define WINDOW_SIZE 10

// Scaling
#define DEG_TO_RAD (M_PI / 180.0f)
#define MDPS_TO_DPS (1.0f / 1000.0f)
#define SENSTIVITY 17.5*1000 // for 500 dps
#define SCALING_FACTOR (SENSTIVITY * DEG_TO_RAD * MDPS_TO_DPS)

typedef struct {
    int16_t raw_x, raw_y, raw_z;
    int16_t avg_x, avg_y, avg_z;
} GestureData;

#define CTRL_REG1 0x20
#define CTRL_REG1_CONFIG 0b01'10'1'1'1'1

// Control Register 4
#define CTRL_REG4 0x23
#define CTRL_REG4_CONFIG 0b0'0'01'0'00'0

// Control Register 3
#define CTRL_REG3 0x22
#define CTRL_REG3_CONFIG 0b0'0'0'0'1'000

// Output Register --> X axis
#define OUT_X_L 0x28

int window_index = 0;
int16_t window_x[WINDOW_SIZE], window_y[WINDOW_SIZE], window_z[WINDOW_SIZE] = {0};

extern GestureData gd={0}; 
#endif