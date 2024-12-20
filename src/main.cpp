#include <mbed.h>

//NOTE: The gyroscope is next to the LCD screen.

const uint8_t spacing = 20; // Adjust spacing for characters
#define PI 3.14159

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

// Define Flag bits for the EventFlags object
#define SPI_FLAG 1
#define DATA_READY_FLAG 2

// Window Size for Moving Average Window
#define WINDOW_SIZE 10

// EventFlags Object Declaration
EventFlags flags;

// SPI
SPI spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel);
uint8_t write_buf[32], read_buf[32];
    


// Callback function for SPI Transfer Completion
void spi_cb(int event) {
    flags.set(SPI_FLAG);
}

// Callback function for Data Ready Interrupt
void data_cb() {
    flags.set(DATA_READY_FLAG);
}

int16_t raw_gx, raw_gy, raw_gz; // raw gyro values
float gx, gy, gz; // converted gyro values
int16_t window_gx[WINDOW_SIZE] = {0}, window_gy[WINDOW_SIZE] = {0}, window_gz[WINDOW_SIZE] = {0};
int window_index = 0;

void setup_spi(){
    spi.format(8, 3);
    spi.frequency(1'000'000);
}

void gyro_register_config(){
    // 1. Control Register 1 
    write_buf[0] = CTRL_REG1;
    write_buf[1] = CTRL_REG1_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    flags.wait_all(SPI_FLAG);

    // 2. Control Register 4
    write_buf[0] = CTRL_REG4;
    write_buf[1] = CTRL_REG4_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    flags.wait_all(SPI_FLAG);

    // 3. Control Register 3
    write_buf[0] = CTRL_REG3;
    write_buf[1] = CTRL_REG3_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    flags.wait_all(SPI_FLAG);

    write_buf[1] = 0xFF;
}

void gyro_get_data(){
    write_buf[0] = OUT_X_L | 0x80 | 0x40;
    spi.transfer(write_buf, 7, read_buf, 7, spi_cb);
    flags.wait_all(SPI_FLAG);
    
    raw_gx = (read_buf[2] << 8) | read_buf[1];
    raw_gy = (read_buf[4] << 8) | read_buf[3];
    raw_gz = (read_buf[6] << 8) | read_buf[5];
}

void print_sensor_data(int16_t x, int16_t y, int16_t z){
    printf(">X axis-> gx: %d|g\n", x);
    printf(">Y axis-> gy: %d|g\n", y);
    printf(">Z axis-> gz: %d|g\n", z);
    printf("----------------\n");
}

void gyro_filtering(){
        int32_t avg_gx = 0, avg_gy = 0, avg_gz = 0;
            
        //---shift all values in array to the left
        for (int i = 1; i < WINDOW_SIZE; i++) {
            window_gx[i-1] = window_gx[i];
            avg_gx += window_gx[i];//adding values to the average sum
        }
        avg_gx += raw_gx;//adding extra current reading
        window_gx[WINDOW_SIZE-1] = raw_gx;    //placed current reading in last spot of array
        avg_gx /= WINDOW_SIZE;//average value calculated

        for(int j = 0; j < 10; j++){
             printf("%d\n", window_gx[j]);
        }

        print_sensor_data(avg_gx, avg_gy, avg_gz);
}

int main() {
    // Interrupt
    InterruptIn int2(PA_2, PullDown);
    int2.rise(&data_cb);

    setup_spi();

    gyro_register_config();

    while (1) {
        flags.wait_all(DATA_READY_FLAG, 0xFF, true);
        
        gyro_get_data();
        gyro_filtering();
    }
}