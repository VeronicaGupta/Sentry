#include <mbed.h>
#include "gyro.h"
#include "filter.h"
#include "spi_config.h"
#include "lcd_out.h"

BufferedSerial pc(USBTX, USBRX, 9600);  // TX, RX, initial baud rate (default 9600)

// Callback function for SPI Transfer Completion
void spi_cb(int event) {
    flags.set(SPI_FLAG);
}

// Callback function for Data Ready Interrupt
void data_cb() {
    flags.set(DATA_READY_FLAG);
}

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
    
    gd.raw_x = ((read_buf[2] << 8) | read_buf[1]);
    gd.raw_y = ((read_buf[4] << 8) | read_buf[3]);
    gd.raw_z = ((read_buf[6] << 8) | read_buf[5]);
}

void print_sensor_data(int16_t x, int16_t y, int16_t z){
    printf(">X axis-> gx: %d|g\n", x);
    printf(">Y axis-> gy: %d|g\n", y);
    printf(">Z axis-> gz: %d|g\n", z);
    printf("----------------\n");
}

void gyro_filtering(){
            
        //---shift all values in array to the left
        for (int i = 1; i < WINDOW_SIZE; i++) {
            window_x[i-1] = window_x[i];
            window_y[i-1] = window_y[i];
            window_z[i-1] = window_z[i];

            gd.avg_x += window_x[i];
            gd.avg_y += window_y[i];
            gd.avg_z += window_z[i];
        }

        gd.avg_x += gd.raw_x;
        window_x[WINDOW_SIZE-1] = gd.raw_x;   
        gd.avg_x /= WINDOW_SIZE;

        gd.avg_y += gd.raw_y;
        window_y[WINDOW_SIZE-1] = gd.raw_y;   
        gd.avg_y /= WINDOW_SIZE;

        gd.avg_z += gd.raw_z;
        window_z[WINDOW_SIZE-1] = gd.raw_z;   
        gd.avg_z /= WINDOW_SIZE;

        if (gd.avg_x<100){
            gd.avg_x=0;
        }

        if (gd.avg_y<100){
            gd.avg_y=0;
        }

        if (gd.avg_z<100){
            gd.avg_z=0;
        }
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

        print_sensor_data(gd.avg_x, gd.avg_y, gd.avg_z);

        display_snowman();
        thread_sleep_for(5000); // Wait 5 seconds
        display_christmas_tree();
        thread_sleep_for(5000); // Wait 5 seconds
        display_loading_screen(); // Continuous animation
    }
}