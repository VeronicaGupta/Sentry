#include <mbed.h>

//NOTE: The gyroscope is next to the LCD screen.

const uint8_t spacing = 20; // Adjust spacing for characters
#define PI 3.14159

// --------------------------STEP 1: Initializations and Definitions-----------------------------
// Define control register addresses and configurations --> Gyroscope!
// Control Register 1
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

// Scaling Factor for data conversion dps --> rps (make sure its the right vale?!)
//#define SCALING_FACTOR (17.5f * 0.0174532925199432957692236907684886f / 1000.0f)

// Window Size for Moving Average Window
#define WINDOW_SIZE 10

// Filter Coefficient for lpf and hpf
#define FILTER_COEFFICIENT 0.1f // Adjust this coefficient as needed!


// EventFlags Object Declaration
EventFlags flags;

// Callback function for SPI Transfer Completion
void spi_cb(int event) {
    flags.set(SPI_FLAG);
}

// Callback function for Data Ready Interrupt
void data_cb() {
    flags.set(DATA_READY_FLAG);
}

// Variable Definitions for Filters MUST BE SIGNED.
    int16_t raw_gx, raw_gy, raw_gz; // raw gyro values
    float gx, gy, gz; // converted gyro values
    // float filtered_gx = 0.0f, filtered_gy = 0.0f, filtered_gz = 0.0f; //lpf filtered values    
    // float high_pass_gx = 0.0f, high_pass_gy = 0.0f, high_pass_gz = 0.0f; //hpf filtered values

// --------------------------END OF STEP 1: Initializations and Definitions-----------------------------
int main() {
    // ---------------------------- STEP 2: SPI and Interrupt Initialization---------------
    
    // SPI
    SPI spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel);
    uint8_t write_buf[32], read_buf[32];
    
    // Interrupt
    InterruptIn int2(PA_2, PullDown);
    int2.rise(&data_cb);

    // SPI Data transmission format and frequency
    spi.format(8, 3);
    spi.frequency(1'000'000);

    // STEP 3: GYRO Configuration!
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

    // dUMMY BYTE for write_buf[1] --> Placeholder Value!
    // We have to send an address and a value for write operation!
    // We have the address but have to send a placeholder value as well!
    write_buf[1] = 0xFF;
    // -----------------------END OF STEP 2-------------------------

    //MUST BE SIGNED
    int i = 0;
    int16_t window_gx[WINDOW_SIZE] = {0}, window_gy[WINDOW_SIZE] = {0}, window_gz[WINDOW_SIZE] = {0};
        int window_index = 0;

    // STEP 4: Loop for Data Collection and Filtering!
    // -----------------------WHILE LOOP ----------------------
    // picked 100 so that data can be analyzed through serial monitor, did NOT use teleplot
    //also did just x-axis simply for testing. It was easier for me to track improvements by focusing on one axis
    while (i<100) {
 
        flags.wait_all(DATA_READY_FLAG, 0xFF, true);
        
        // Read GYRO Data using SPI transfer --> 6 bytes!
        write_buf[0] = OUT_X_L | 0x80 | 0x40;
        spi.transfer(write_buf, 7, read_buf, 7, spi_cb);
        flags.wait_all(SPI_FLAG);
        //combine high and low registers to one uint16_t variable to get raw values.
        raw_gx = (read_buf[2] << 8) | read_buf[1];
        raw_gy = (read_buf[4] << 8) | read_buf[3];
        raw_gz = (read_buf[6] << 8) | read_buf[5];
        //--------------------------------------- COLLECTION OF RAW DATA ENDS ---------------------------

        //-----------------------START OF MOVING AVERAGE ------------------------------
        //MY DESIGN OF MOVING AVERAGE FILTER (Using Queue)
        int32_t avg_gx = 0, avg_gy = 0, avg_gz = 0;
            
        //---shift all values in array to the left
        for (int i = 1; i < WINDOW_SIZE; i++) {
            window_gx[i-1] = window_gx[i];
            avg_gx += window_gx[i];//adding values to the average sum
        }
        avg_gx += raw_gx;//adding extra current reading
        window_gx[WINDOW_SIZE-1] = raw_gx;    //placed current reading in last spot of array
        avg_gx /= WINDOW_SIZE;//average value calculated


        // Printing average, raw point, and then the window array
        printf("Average data point: %d\n", avg_gx);
        printf("Raw data point: %d\n", raw_gx);

        printf("Here is the array from first element to last: \n");
        for(int j = 0; j < 10; j++){
             printf("%d\n", window_gx[j]);
        }
       
        
        // printf("%u\n", avg_gy);
        // printf("%u\n", avg_gz);
        printf("----------------\n"); // to more easily read serial monitor

        i++; // to end while loop at some point
    }
}
