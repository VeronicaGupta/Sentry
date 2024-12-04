#include "mbed.h"

// Pin Definitions
DigitalOut greenLED(LED1);   // Green LED for success
DigitalOut redLED(LED2);     // Red LED for failure
DigitalIn recordButton(USER_BUTTON); // Button for recording gesture
DigitalIn unlockButton(PA_0);        // Button for unlocking

// IMU Data Simulation (for simplicity, replace with actual IMU readings)
float recordedAccelX[100], recordedAccelY[100], recordedAccelZ[100];
float testAccelX[100], testAccelY[100], testAccelZ[100];
int dataCount = 0

// State Enum
enum State { IDLE, RECORDING, UNLOCKING } currentState = IDLE;

// Function Prototypes
void recordGesture();
bool compareGesture();
void indicateSuccess();
void indicateFailure();

int main() {
    greenLED = 0;  // Turn off LEDs initially
    redLED = 0;

    while (1) {
        if (recordButton == 1) {
            currentState = RECORDING;
            recordGesture();
            currentState = IDLE;
        }

        if (unlockButton == 1) {
            currentState = UNLOCKING;
            if (compareGesture()) {
                indicateSuccess();
            } else {
                indicateFailure();
            }
            currentState = IDLE;
        }
    }
}

// Function to record a gesture
void recordGesture() {
    dataCount = 0;
    for (int i = 0; i < 100; i++) {
        recordedAccelX[i] = i * 0.1;  // Simulated data
        recordedAccelY[i] = i * 0.2;
        recordedAccelZ[i] = i * 0.3;
        dataCount++;
        wait_us(50000); // Simulate a 50ms delay (sampling rate of 20Hz)
    }
    greenLED = 1; // Indicate recording complete
    wait(0.5);
    greenLED = 0;
}

// Function to compare recorded gesture with test gesture
bool compareGesture() {
    for (int i = 0; i < dataCount; i++) {
        testAccelX[i] = i * 0.1;  // Simulated test data
        testAccelY[i] = i * 0.2;
        testAccelZ[i] = i * 0.3;

        if (abs(recordedAccelX[i] - testAccelX[i]) > 0.05 ||
            abs(recordedAccelY[i] - testAccelY[i]) > 0.05 ||
            abs(recordedAccelZ[i] - testAccelZ[i]) > 0.05) {
            return false; // Data does not match
        }
    }
    return true;
}

// Function to indicate success
void indicateSuccess() {
    for (int i = 0; i < 3; i++) {
        greenLED = 1;
        wait(0.5);
        greenLED = 0;
        wait(0.5);
    }
}

// Function to indicate failure
void indicateFailure() {
    for (int i = 0; i < 3; i++) {
        redLED = 1;
        wait(0.5);
        redLED = 0;
        wait(0.5);
    }
}
