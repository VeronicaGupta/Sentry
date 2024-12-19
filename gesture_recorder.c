#include "gesture_recorder.h"
#include "stm32f4xx_hal.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

GestureData record_gesture(int duration) {
    GestureData gesture = { .length = 0 };
    uint32_t start_time = HAL_GetTick();

    while ((HAL_GetTick() - start_time) < (duration * 1000)) {
        SensorData data;
        MPU6050_ReadGyro(&hi2c1, &data.Gx, &data.Gy, &data.Gz);
        MPU6050_ReadAccel(&hi2c1, &data.Ax, &data.Ay, &data.Az);
        gesture.data[gesture.length++] = data;
        HAL_Delay(100);
    }

    return gesture;
}

void save_gesture(const GestureData* gesture) {
    // Simulate saving to flash or external storage
}

GestureData load_gesture(void) {
    // Simulate loading from flash or external storage
    GestureData gesture;
    memset(&gesture, 0, sizeof(gesture));
    return gesture;
}
