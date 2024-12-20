#include <mbed.h>
#include "gyro.h"
#include "filter.h"
#include "spi_config.h"
#include "lcd_out.h"
#include "FATfilesystem.h"
#include "LittleFileSystem.h"
#include "BlockDevice.h"

BufferedSerial pc(USBTX, USBRX, 9600);  // TX, RX, initial baud rate (default 9600)

// Create a block device and filesystem instance
BlockDevice *bd = BlockDevice::get_default_instance();
LittleFileSystem fs("fs");

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

void print_sensor_data(int16_t x, int16_t y, int16_t z){
    printf(">X axis-> gx: %d|g\n", x);
    printf(">Y axis-> gy: %d|g\n", y);
    printf(">Z axis-> gz: %d|g\n", z);
    printf("----------------\n");
}

void gyro_filtering(){
            
        //---shift all values in array to the left
        for (int i = 0; i < WINDOW_SIZE; i++) {
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

        if (abs(gd.avg_x)<50){
            gd.avg_x=0;
        }

        if (abs(gd.avg_y)<50){
            gd.avg_y=0;
        }

        if (abs(gd.avg_z)<50){
            gd.avg_z=0;
        }
}

void gyro_get_data(){
    write_buf[0] = OUT_X_L | 0x80 | 0x40;
    spi.transfer(write_buf, 7, read_buf, 7, spi_cb);
    flags.wait_all(SPI_FLAG);
    
    gd.raw_x = ((read_buf[2] << 8) | read_buf[1]);
    gd.raw_y = ((read_buf[4] << 8) | read_buf[3]);
    gd.raw_z = ((read_buf[6] << 8) | read_buf[5]);

    gyro_filtering();

    // print_sensor_data(gd.avg_x, gd.avg_y, gd.avg_z);

}

void read_gesture_data(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", strerror(errno));
        return;
    }

    char buffer[2048]; // Ensure buffer size is large enough for your data
    size_t read_size = fread(buffer, 1, sizeof(buffer) - 1, file);
    if (read_size == 0) {
        printf("Error reading file or file is empty\n");
        fclose(file);
        return;
    }
    buffer[read_size] = '\0'; // Null-terminate the buffer
    fclose(file);

    // Parse JSON data
    char* ptr = buffer;

    // Skip the opening '['
    ptr = strchr(ptr, '[');
    if (!ptr) {
        printf("Error: Invalid JSON format\n");
        return;
    }
    ptr++;

    for (int i = 0; i < SAMPLE; i++) {
        int x, y, z;
        
        // Skip spaces and newlines
        while (*ptr == '\n' || *ptr == ' ' || *ptr == ',') {
            ptr++;
        }

        // Parse the object
        if (sscanf(ptr, "{\"x\": %d, \"y\": %d, \"z\": %d}", &x, &y, &z) == 3) {
            gd_saved.avg_x[i] = x;
            gd_saved.avg_y[i] = y;
            gd_saved.avg_z[i] = z;

            printf("Parsed Point %d: X=%d, Y=%d, Z=%d\n", i, gd_saved.avg_x[i], gd_saved.avg_y[i], gd_saved.avg_z[i]);

            // Move pointer forward to the next data point
            ptr = strchr(ptr, '}');
            if (ptr) {
                ptr++; // Move past the closing brace
            }
        } else {
            printf("Error parsing JSON data at index %d\n", i);
            break;
        }
    }
}


EventQueue queue(32 * EVENTS_EVENT_SIZE);
EventQueue queue1(32 * EVENTS_EVENT_SIZE);
Thread event_thread, event_thread1;

void record_data_task();
void button_press_record_handler() {
    queue.call(record_data_task);
}

void unlock_data_task(){
    display_snowman("Test Gesture..");
}


void button_press_unlock_handler() {
    queue1.call(unlock_data_task);
}

void record_data_task() {
    // Perform file operations and data recording here
    int count=0;
    // Check if file exists
    if (FILE* file = fopen(filename, "r")) {
        fclose(file);
        // File exists, so remove it
        if (remove(filename) != 0) {
            printf("Error deleting existing file\n");
            return;
        }
    }
    // Create new file
    FILE* file = fopen(filename, "w+");
    if (!file) {
        printf("Error creating config file: %s\n", strerror(errno));
        return;
    }
    fprintf(file, "[\n");
    display_loading_screen("Gesture Recording..");
    while (count < SAMPLE){        
        if (file != NULL) {
            fprintf(file, "    {\"x\": %ld, \"y\": %ld, \"z\": %ld}", gd.avg_x, gd.avg_y, gd.avg_z);
            printf("Recording Point %d: X=%ld, Y=%ld, Z=%ld\n", count, gd.avg_x, gd.avg_y, gd.avg_z);
            if (count != SAMPLE-1) {  // Add a comma except for the last item
                fprintf(file, ",");
            }
            count++;
        } else {
            printf("Error writing in file: %s\n", strerror(errno));
        }
        ThisThread::sleep_for(10ms);
    }
    fprintf(file, "\n]");
    fclose(file);
    read_gesture_data(filename);
    
    for (int i = 0; i < SAMPLE; i++) {
        printf("Point %d: X=%d, Y=%d, Z=%d\n", 
        i, gd_saved.avg_x[i], gd_saved.avg_y[i], gd_saved.avg_z[i]);
    }
    display_snowman("Recorded!,Unlock Now..");

    button.rise(&button_press_unlock_handler); // unlock button
    event_thread1.start(callback(&queue1, &EventQueue::dispatch_forever));

    return;
}

int main() {
    int2.rise(&data_cb); // gyro sensor
    button.rise(&button_press_record_handler); // record button
    event_thread.start(callback(&queue, &EventQueue::dispatch_forever));

    setup_spi();
    gyro_register_config();
    gyro_get_data();

    // Mount the filesystem
    int err = fs.mount(bd);
    if (err) {
        // If mounting fails, format and mount again
        printf("Formatting the filesystem...\n");
        err = fs.format(bd);
        if (err) {
            printf("Filesystem format failed\n");
            return -1;
        }
        err = fs.mount(bd);
        if (err) {
            printf("Filesystem mount failed\n");
            return -1;
        }
    }

    display_snowman("Press Blue button");

    while (1) {
        flags.wait_all(DATA_READY_FLAG, 0xFF, true);

        gyro_get_data();

        thread_sleep_for(100);
    }

    fs.unmount();
    return 0;
}