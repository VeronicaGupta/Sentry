#include "mbed.h"

BufferedSerial pc(USBTX, USBRX, 9600);  // TX, RX, initial baud rate (default 9600)

// EventFlags
EventFlags flags;
#define SPI_FLAG 1
#define DATA_READY_FLAG 2

#define DEG_TO_RAD (M_PI / 180.0f)
#define MDPS_TO_DPS (1.0f / 1000.0f)
#define SENSITIVITY 17.5
#define SCALING_FACTOR (SENSITIVITY * DEG_TO_RAD * MDPS_TO_DPS)
#define INT_SCALE 65536 
#define SCALE (500*2)

// Window Size for Moving Average Window
#define WINDOW_SIZE 10

// Filter Coefficient for lpf and hpf
#define FILTER_COEFFICIENT 0.2f // Adjust this coefficient as needed!


// Registers settings
#define CTRL_REG1 0x20
#define CTRL_REG1_CONFIG 0b01111111     // Enable all axes, set ODR = 100 Hz
// #define CTRL_REG2 0x21
// #define CTRL_REG2_CONFIG 0b00000001     // Enable high-pass filter, mode = 01
#define CTRL_REG3 0x22
#define CTRL_REG3_CONFIG 0b0'000'1000     // Enable interrupt signal on INT1 pin
#define CTRL_REG4 0x23
#define CTRL_REG4_CONFIG 0b0'001'0000     // Selecting 500 dps full scale
#define CTRL_REG5 0x24
#define CTRL_REG5_CONFIG 0b0'000'0100     // Selecting 

#define INT1_CFG 0x30
#define INT1_CFG_CONFIG 0b0'000'0111      // Enable interrupt for motion detection on X, Y, Z axes
#define INT1_THS_XL 0x33
#define INT1_THS_XL_CONFIG 0b0'000'0100   // Threshold for X-axis
#define INT1_THS_YL 0x35
#define INT1_THS_YL_CONFIG 0b0'000'0100   // Threshold for Y-axis
#define INT1_THS_ZL 0x37
#define INT1_THS_ZL_CONFIG 0b0'000'0100   // Threshold for Z-axis
#define INT1_DURATION 0x38
#define INT1_DURATION_CONFIG 0b1'000'0101 // Threshold wait duration = 5 ODR cycles
#define INT1_SRC 0x31

#define OUT_X_L 0x28

// Callback function for SPI Transfer Completion
void spi_cb(int event) {
    flags.set(SPI_FLAG);
}

// Callback function for Data Ready Interrupt
void data_cb() {
    flags.set(DATA_READY_FLAG);
}

// Variable Definitions for Filters
uint16_t raw_gx, raw_gy, raw_gz; // raw gyro values
float gx = 0.0f, gy = 0.0f, gz = 0.0f; // converted gyro values
float avg_gx = 0.0f, avg_gy = 0.0f, avg_gz = 0.0f;
float gx_prev = 0.0f, gy_prev = 0.0f, gz_prev = 0.0f; // converted gyro values
float filtered_gx = 0.0f, filtered_gy = 0.0f, filtered_gz = 0.0f; //lpf filtered values    
float high_pass_gx = 0.0f, high_pass_gy = 0.0f, high_pass_gz = 0.0f; //hpf filtered values

// Moving Average Filter Buffer --> only to be used with the Moving Average Filter!
float window_gx[WINDOW_SIZE] = {0}, window_gy[WINDOW_SIZE] = {0}, window_gz[WINDOW_SIZE] = {0};
int window_index = 0;

// FIR LPF Coefficients --> only to be used for FIR LPF!
#define FIR_SIZE 5
float fir_coefficients[FIR_SIZE] = {FILTER_COEFFICIENT, FILTER_COEFFICIENT, FILTER_COEFFICIENT, FILTER_COEFFICIENT, FILTER_COEFFICIENT};
float fir_buffer[FIR_SIZE] = {0};
int fir_buffer_index = 0;

// Functions for FIR filters --> only to be used for FIR LPF!
float fir_filter(float input) {
    fir_buffer[fir_buffer_index] = input;
    float output = 0.0f;
    for (int i = 0; i < FIR_SIZE; i++) {
        output += fir_coefficients[i] * fir_buffer[(fir_buffer_index - i + FIR_SIZE) % FIR_SIZE];
    }
    fir_buffer_index = (fir_buffer_index + 1) % FIR_SIZE;
    return output;
}

void detectGesture(SPI spi, uint8_t* write_buf, uint8_t* read_buf) {
    write_buf[0] = INT1_SRC | 0x80;  // Read INT1_SRC register
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);

    uint8_t int1_src = read_buf[1];
    if (int1_src & 0x01) {
        printf("X-axis motion detected\n");
    }
    if (int1_src & 0x02) {
        printf("Y-axis motion detected\n");
    }
    if (int1_src & 0x04) {
        printf("Z-axis motion detected\n");
    }
}

int process_axis(float *x, float *y, float *z){

    *x = (float) (*x * SCALE) / INT_SCALE;
    *y = (float) (*y * SCALE) / INT_SCALE;
    *z = (float) (*z * SCALE) / INT_SCALE;
}

int main() {
    printf("Program Started\n");

    // SPI Init
    SPI spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel); // MOSI, MISO, SCK, CS
    uint8_t write_buf[32], read_buf[32];

    // Interrupt
    InterruptIn int2(PA_2, PullDown);
    int2.rise(&data_cb);

    // SPI Data transmission format and frequency
    spi.format(8, 3);
    spi.frequency(1'000'000);

    // Verify SPI Communication
    write_buf[0] = CTRL_REG1;
    write_buf[1] = CTRL_REG1_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    flags.wait_all(SPI_FLAG);

    // Configure CTRL_REG4 to set full-scale range
    write_buf[0] = CTRL_REG3;
    write_buf[1] = CTRL_REG3_CONFIG;  // Enable all axes and set ODR
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    flags.wait_all(SPI_FLAG);
    
    // Configure CTRL_REG4 to set full-scale range
    write_buf[0] = CTRL_REG4;
    write_buf[1] = CTRL_REG4_CONFIG;  // Enable all axes and set ODR
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    flags.wait_all(SPI_FLAG);

    // // Configure Thresholds and waitime
    // write_buf[0] = CTRL_REG5;
    // write_buf[1] = CTRL_REG5_CONFIG; 
    // spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    // flags.wait_all(SPI_FLAG);

    // write_buf[0] = INT1_THS_XL;
    // write_buf[1] = INT1_THS_XL_CONFIG;  // Threshold for X-axis
    // spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    // flags.wait_all(SPI_FLAG);

    // write_buf[0] = INT1_THS_YL;
    // write_buf[1] = INT1_THS_YL_CONFIG;  // Threshold for Y-axis
    // spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    // flags.wait_all(SPI_FLAG);

    // write_buf[0] = INT1_THS_ZL;
    // write_buf[1] = INT1_THS_ZL_CONFIG;  // Threshold for Z-axis
    // spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    // flags.wait_all(SPI_FLAG);

    // write_buf[0] = INT1_DURATION;
    // write_buf[1] = INT1_DURATION_CONFIG;  // Wait duration in ODR cycles
    // spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    // flags.wait_all(SPI_FLAG);


    while (1) {
        printf("Waiting for Data Ready...\n");
        flags.wait_all(DATA_READY_FLAG, 0xFF, true);

        write_buf[0] = OUT_X_L | 0x80 | 0x40; // Auto-increment read
        spi.transfer(write_buf, 7, read_buf, 7, &spi_cb);
        flags.wait_all(SPI_FLAG);

        // Extract raw 16-bit gyroscope data for X, Y, Z
        raw_gx = ((uint16_t) read_buf[2] << 8) | read_buf[1];
        raw_gy = ((uint16_t) read_buf[4] << 8) | read_buf[3];
        raw_gz = ((uint16_t) read_buf[6] << 8) | read_buf[5];

        // // Convert raw data to radians per second!
        // gx = raw_gx * SCALING_FACTOR;
        // gy = raw_gy * SCALING_FACTOR;
        // gz = raw_gz * SCALING_FACTOR;

        gx = (float) ( (raw_gx / INT_SCALE) * SCALE);
        gy = (float) ( (raw_gy / INT_SCALE) * SCALE);
        gz = (float) ( (raw_gz / INT_SCALE) * SCALE);

        // gx = process_axis(gx_prev, gx);
        // gy = process_axis(gy_prev, gy);
        // gz = process_axis(gz_prev, gz);

        // window_gx[window_index] = gx;
        // window_gy[window_index] = gy;
        // window_gz[window_index] = gz;
        // for (int i = 0; i < WINDOW_SIZE; i++) {
        //     avg_gx += window_gx[i];
        //     avg_gy += window_gy[i];
        //     avg_gz += window_gz[i];
        // }
        // avg_gx /= WINDOW_SIZE;
        // avg_gy /= WINDOW_SIZE;
        // avg_gz /= WINDOW_SIZE;
        // window_index = (window_index + 1) % WINDOW_SIZE;
        // printf("Moving Average -> gx: %4.5f, gy: %4.5f, gz: %4.5f\n", avg_gx, avg_gy, avg_gz);
        // printf(">Moving Average X axis-> gx: %4.5f|g\n", avg_gx);
        // printf(">Moving Average Y axis-> gy: %4.5f|g\n", avg_gy);
        // printf(">Moving Average Z axis-> gz: %4.5f|g\n", avg_gz);

        // gx = avg_gx;
        // gy = avg_gy;
        // gz = avg_gz;

        // filtered_gx = FILTER_COEFFICIENT * gx + (1 - FILTER_COEFFICIENT) * filtered_gx;
        // filtered_gy = FILTER_COEFFICIENT * gy + (1 - FILTER_COEFFICIENT) * filtered_gy;
        // filtered_gz = FILTER_COEFFICIENT * gz + (1 - FILTER_COEFFICIENT) * filtered_gz;
        // printf("LPF IIR -> gx: %4.5f, gy: %4.5f, gz: %4.5f\n", filtered_gx, filtered_gy, filtered_gz);
        // printf(">LPF IIR X axis-> gx: %4.5f|g\n", filtered_gx);
        // printf(">LPF IIR Y axis-> gy: %4.5f|g\n", filtered_gy);
        // printf(">LPF IIR Z axis-> gz: %4.5f|g\n", filtered_gz);

        printf("Axis -> gx: %4.5f, gy: %4.5f, gz: %4.5f\n", gx, gy, gz);
        printf(">X axis-> gx: %4.5f|g\n", gx);
        printf(">Y axis-> gy: %4.5f|g\n", gy);
        printf(">Z axis-> gz: %4.5f|g\n", gz);

        // printf(">X axis-> gx: %4.5f|g\n", fir_filter(gx));
        // printf(">Y axis-> gy: %4.5f|g\n", fir_filter(gy));
        // printf(">Z axis-> gz: %4.5f|g\n", fir_filter(gy));

        gx_prev = gx;
        gy_prev = gy;
        gz_prev = gz;

        thread_sleep_for(50);
    }
}









