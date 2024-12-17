#include "mbed.h"
#include "MPU6050.h" // Example IMU library, replace with your sensor library

I2C i2c(PB_9, PB_8); // SDA, SCL
MPU6050 imu(i2c);

struct SensorData {
    float Gx, Gy, Gz;
    float Ax, Ay, Az;
};

SensorData read_sensor_data() {
    SensorData data;
    imu.getGyro(&data.Gx, &data.Gy, &data.Gz);
    imu.getAccel(&data.Ax, &data.Ay, &data.Az);
    return data;
}

int main() {
    imu.initialize();
    while (true) {
        SensorData data = read_sensor_data();
        printf("Gx: %.2f, Gy: %.2f, Gz: %.2f, Ax: %.2f, Ay: %.2f, Az: %.2f\n", 
                data.Gx, data.Gy, data.Gz, data.Ax, data.Ay, data.Az);
        thread_sleep_for(100);
    }
}
