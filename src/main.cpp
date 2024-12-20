#include <mbed.h>

//-------------for pointwise comparision
//uint32_t dtw_cost(int16_t[30], int16_t[30]);
//uint32_t find_min(uint32_t, uint32_t, uint32_t);
//This version only uses the x axis since I am testing on a small scale
//I used the distance calculation and a range of 26,000 or less is pretty close
//50,000  or less is close as well 
// anywhere above or especially close to 100,000 or 90,000 is not at all close
//Way at the bottom is the early steps of the recognition algorithm

//-----------------for DTW
//unsure but anything below 700,000 appears to be close. But anything lower is even better
//unsure if we should use this algorithm, maybe using it with other axis will help

//NOTE: The gyroscope is next to the LCD screen.

#include <math.h>

const uint8_t spacing = 20; // Adjust spacing for characters
#define PI 3.14159
#define SET_SIZE 30

//--------------------------FUNCTIONS--------------------------------------

uint32_t find_min(uint32_t v1, uint32_t v2, uint32_t v3){
    uint32_t minimum = (v1 < v2) ? ((v1 < v3) ? v1 : v3) : ((v2 < v3) ? v2 : v3); 
    return minimum;
}

uint32_t dtw_cost(int16_t (&set_one)[SET_SIZE], int16_t (&set_two)[SET_SIZE]){
    uint32_t dtw_mat[SET_SIZE][SET_SIZE] = {0};

    uint32_t cost = 0;
    int i;
    //calculate first lowest, leftest element of dtw----------------------
    if((set_one[0] > 0) && (set_two[0] <0)){
            dtw_mat[0][0] = abs(abs(set_one[0]) + abs(set_two[0]));
        }
        
    else if ((set_one[0] < 0) && (set_two[0] >0))
        {
            dtw_mat[0][0] = abs(abs(set_one[0]) + abs(set_two[0]));
        }
        
    else{
            dtw_mat[0][0] = abs(abs(set_one[0]) - abs(set_two[0]));
        }
    //calculate first column now--------------------------
    for(i = 1; i < SET_SIZE; i++){
        if((set_one[i] > 0) && (set_two[0] <0))
            dtw_mat[i][0] = abs(abs(set_one[i]) + abs(set_two[0])) + dtw_mat[i-1][0];
        
        else if ((set_one[0] < 0) && (set_two[0] >0))
            dtw_mat[i][0] = abs(abs(set_one[i]) + abs(set_two[0])) + dtw_mat[i-1][0];

        else
            dtw_mat[i][0] = abs(abs(set_one[i]) - abs(set_two[0])) + dtw_mat[i-1][0];
        
    }

    //now calculate the first row
    for(i = 1; i < SET_SIZE; i++){
        if((set_one[0] > 0) && (set_two[i] <0))
            dtw_mat[0][i] = abs(abs(set_one[0]) + abs(set_two[i])) + dtw_mat[0][i-1];
        
        else if ((set_one[0] < 0) && (set_two[0] >0))
            dtw_mat[0][i] = abs(abs(set_one[0]) + abs(set_two[i])) + dtw_mat[0][i-1];

        else
            dtw_mat[0][i] = abs(abs(set_one[0]) - abs(set_two[i])) + dtw_mat[0][i-1];
        
    }

    //now since first row and column are finished. Calculate the rest of the matrix
    for(i = 1; i < SET_SIZE; i++){
        for(int j = 1; j < SET_SIZE; j++){
            if((set_one[i] > 0) && (set_two[j] <0))
            dtw_mat[i][j] = abs(abs(set_one[i]) + abs(set_two[j])) + find_min(dtw_mat[i-1][j-1], dtw_mat[i-1][j], dtw_mat[i][j-1]);
        
        else if ((set_one[i] < 0) && (set_two[j] >0))
            dtw_mat[i][j] = abs(abs(set_one[i]) + abs(set_two[j])) + find_min(dtw_mat[i-1][j-1], dtw_mat[i-1][j], dtw_mat[i][j-1]);

        else
            dtw_mat[i][j] = abs(abs(set_one[i]) - abs(set_two[j])) + find_min(dtw_mat[i-1][j-1], dtw_mat[i-1][j], dtw_mat[i][j-1]);
        }
    }
    //matrix has been printed for testing
    for(i = SET_SIZE-1; i >=0; i--){
        for(int j = 0; j < SET_SIZE; j++){
            printf("%lu ", dtw_mat[i][j]);
        }
        printf("\n");
    }
    
    //--------------------------create the cost path and check!
    cost += dtw_mat[SET_SIZE -1][SET_SIZE -1]; // add the last spot of the array
    int col = SET_SIZE -1;
    int row = SET_SIZE -1;
    uint32_t val = 0;
    for(i = SET_SIZE-2; i >= 0; i--){ //its minus 2 because one is already added and the extra is for the array limit
        val = find_min(dtw_mat[row-1][col-1], dtw_mat[row-1][col], dtw_mat[row][col-1]);
        if (val == dtw_mat[row-1][col-1]){
            row--;
            col--;
        }

        else if (dtw_mat[row-1][col]){
            row--;
        }
        

        else{
            col--;
        }
        cost += val;
    }

    return cost;

}



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


    int16_t average_gx_arr[30] = {0}; //for 3 second gesture
    // STEP 4: Loop for Data Collection and Filtering!
    // -----------------------WHILE LOOP ----------------------
    // picked 100 so that data can be analyzed through serial monitor, did NOT use teleplot
    //also did just x-axis simply for testing. It was easier for me to track improvements by focusing on one axis

    while (i<30) {
 
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
        window_gx[WINDOW_SIZE-1] = raw_gx;    //placed reading in last spot of array
        avg_gx /= WINDOW_SIZE;//average value calculated

        //Now storing in array!
        average_gx_arr[i] = avg_gx; //storing data point in the average array!!

        // Printing average, raw point, and then the window array
        printf(">Average data point: %d|g\n", avg_gx);
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
    printf("Starting next gesture!"); //---------------------------------------
    thread_sleep_for(1600);
    //=====================AGAIN----------------------
    i = 0;
    int16_t window_gx_two[WINDOW_SIZE] = {0};


    int16_t average_gx_arr_two[30] = {0};

    while (i<30) {
 
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
            window_gx_two[i-1] = window_gx_two[i];
            avg_gx += window_gx_two[i];//adding values to the average sum
        }
        avg_gx += raw_gx;//adding extra current reading
        window_gx_two[WINDOW_SIZE-1] = raw_gx;    //placed reading in last spot of array
        avg_gx /= WINDOW_SIZE;//average value calculated

        //Now storing in array!
        average_gx_arr_two[i] = avg_gx; //storing data point in the average array!!

        // Printing average, raw point, and then the window array
        printf(">Second Average data point: %dg\n", avg_gx);
        printf("Raw data point: %d\n", raw_gx);

        printf("Here is the array from first element to last: \n");
        for(int j = 0; j < 10; j++){
             printf("%d\n", window_gx_two[j]);
        }
       
        
        // printf("%u\n", avg_gy);
        // printf("%u\n", avg_gz);
        printf("----------------\n"); // to more easily read serial monitor

        i++; // to end while loop at some point
    }

    //-----------------------------------KEY GESTURE IS NOW SAVED THROUGH AVERAGING-----------------
    uint32_t final_sum = 0;
    // //MANHATTEN L1 algorithm ------------------------MOST LIKELY USE THIS ONE
    // for (int k  = 0; k < 30; k++){
    //     if((average_gx_arr[k] > 0) && (average_gx_arr_two[k] <0)){
    //         final_sum += abs(abs(average_gx_arr[k]) + abs(average_gx_arr_two[k]));
    //     }
        
    //     else if ((average_gx_arr[k] < 0) && (average_gx_arr_two[k] > 0))
    //     {
    //         final_sum += abs(abs(average_gx_arr[k]) + abs(average_gx_arr_two[k]));
    //     }
        
    //     else{
    //         final_sum += abs(abs(average_gx_arr[k]) - abs(average_gx_arr_two[k]));

    //     }
    // }

    //reusable dtw matrix (must be reused to save space);
    //first fill whole first row 
    final_sum = dtw_cost(average_gx_arr, average_gx_arr_two);
    printf(">Here is the final sum: %dg\n", final_sum);
}
