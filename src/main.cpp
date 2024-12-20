#include "mbed.h"

// Gyroscope Registers and Configurations
#define CTRL_REG1 0x20
#define CTRL_REG1_CONFIG 0b01101111 // Enable all axes, 200Hz with 50Hz cutoff
#define CTRL_REG4 0x23
#define CTRL_REG4_CONFIG 0b00100000 // Full-scale ±500 dps
#define CTRL_REG3 0x22
#define CTRL_REG3_CONFIG 0b00001000 // Enable DRDY interrupt
#define OUT_X_L 0x28

#define DEG_TO_RAD (M_PI / 180.0f)
#define MDPS_TO_DPS (1.0f / 1000.0f)
#define SENSTIVITY 17.5 // for 500 dps selection
#define SCALING_FACTOR (17.5f * DEG_TO_RAD * MDPS_TO_DPS)

// Filter Configuration
#define WINDOW_SIZE 10
#define FILTER_COEFFICIENT 0.1f

// Define Flag bits for the EventFlags object
#define SPI_FLAG 1
#define DATA_READY_FLAG 2

// SPI
#define SPI_FREQ 1000000

// Globals
EventFlags flags;
SPI spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel); // MOSI, MISO, SCLK, CS
InterruptIn int2(PA_2, PullDown);
float filtered_gx = 0.0f, filtered_gy = 0.0f, filtered_gz = 0.0f; // Filtered values
float gx, gy, gz; // Scaled gyro values
uint16_t raw_gx, raw_gy, raw_gz; // Raw gyro data
uint8_t write_buf[32], read_buf[32];

// Callback for SPI Transfer Completion
void spi_cb(int event) {
    flags.set(SPI_FLAG);
}

// Callback for Data Ready Interrupt
void data_cb() {
    flags.set(DATA_READY_FLAG);
}

/**
 * @brief Initialize the SPI and gyroscope control registers.
 */
void init_gyro() {
    // Configure SPI settings
    spi.format(8, 3);
    spi.frequency(SPI_FREQ);

    // Configure gyroscope control registers
    write_buf[0] = CTRL_REG1;
    write_buf[1] = CTRL_REG1_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    flags.wait_all(SPI_FLAG);

    write_buf[0] = CTRL_REG4;
    write_buf[1] = CTRL_REG4_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    flags.wait_all(SPI_FLAG);

    write_buf[0] = CTRL_REG3;
    write_buf[1] = CTRL_REG3_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, &spi_cb);
    flags.wait_all(SPI_FLAG);

    printf("Gyroscope Initialized.\n");
}

/**
 * @brief Read raw gyroscope data from the gyroscope registers.
 */
void read_gyro_data() {
    write_buf[0] = OUT_X_L | 0x80 | 0x40; // Auto-increment read
    spi.transfer(write_buf, 7, read_buf, 7, spi_cb);
    flags.wait_all(SPI_FLAG);

    raw_gx = ((int16_t)read_buf[2] << 8) | read_buf[1];
    raw_gy = ((int16_t)read_buf[4] << 8) | read_buf[3];
    raw_gz = ((int16_t)read_buf[6] << 8) | read_buf[5];

    gx = raw_gx * SCALING_FACTOR;
    gy = raw_gy * SCALING_FACTOR;
    gz = raw_gz * SCALING_FACTOR;
}

/**
 * @brief Main control loop for reading and processing gyroscope data.
 */
void process_gyro_data() {
    while (true) {
        // Wait for data ready flag
        flags.wait_all(DATA_READY_FLAG, 0xFF, true);

        // Read raw data and apply filtering
        read_gyro_data();
        apply_lpf();

        thread_sleep_for(100);
    }
}

/**
 * @brief Apply an IIR low-pass filter to the gyroscope data.
 */
void apply_lpf() {
    filtered_gx = FILTER_COEFFICIENT * gx + (1 - FILTER_COEFFICIENT) * filtered_gx;
    filtered_gy = FILTER_COEFFICIENT * gy + (1 - FILTER_COEFFICIENT) * filtered_gy;
    filtered_gz = FILTER_COEFFICIENT * gz + (1 - FILTER_COEFFICIENT) * filtered_gz;

    printf("LPF IIR -> gx: %4.5f, gy: %4.5f, gz: %4.5f\n", filtered_gx, filtered_gy, filtered_gz);
    printf(">LPF IIR X axis-> gx: %4.5f|g\n", filtered_gx);
    printf(">LPF IIR Y axis-> gy: %4.5f|g\n", filtered_gy);
    printf(">LPF IIR Z axis-> gz: %4.5f|g\n", filtered_gz);
}

int main() {
    printf("Initializing System...\n");

    // Initialize gyroscope and configure interrupts
    init_gyro();
    int2.rise(&data_cb);

    // Process gyroscope data
    process_gyro_data();

    return 0;
}
