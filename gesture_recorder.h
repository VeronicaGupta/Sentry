#ifndef GESTURE_RECORDER_H
#define GESTURE_RECORDER_H

#include "mpu6050.h"

typedef struct {
    float Gx, Gy, Gz;
    float Ax, Ay, Az;
} SensorData;

typedef struct {
    SensorData data[100];
    size_t length;
} GestureData;

GestureData record_gesture(int duration);
void save_gesture(const GestureData* gesture);
GestureData load_gesture(void);

#endif
