## **Sentry**

### **Overview**
The Sentry challenge involves STM32F429ZI's Gyroscope to capture data, process it and then compare the sequence  via DTW recognition for gesture-based unlocking/locking.

### **Demo Video**
Link: https://youtube.com/shorts/s6QqmGTkBAY?si=GU7sCO6Fnp7VEv-T
Challenge demonstration includes:
  1. Gesture recording.
  2. Gesture testing and unlock.
  3. Deleting gestures.
  4. Live Teleplot visualization.

### **Architecture**:
- **System pattern flow**: Uses queue and button interrupt events for saving/recording gestures.
- **Collection**: Uses in-build gyro sensor via spi communication interrupts for collecting x, y, z axis data.
- **Filtering**: Uses Moving Average FIR (Finite Impulse Response) window filtering for processing axis raw data.
- **Fetching**: Code saves/load a gesture in a file on dev board flash when required to record/match.
- **Screens**: Uses in-built lcd driver for three types of visual feedback: success, failure and loading.
- **Recognition**: Uses Dynamic time warping (DTW) algorithm to detect the matching rate of the gestures.

### **System Design Flow**:
- **Secret gesture recording**: User record a "secret" sequence gesture using a "Record Key."
- **New gesture recording**: User record a "new" sequence gesture using a "Record Key."
- **Gesture matching**: Algo matches the secret and new sequence gesture to "Unlock" if success and remains "Locked" if does not matches.

---

### **Features**
1. **Data Collection**:
   - Captures gyroscope data (`X`, `Y`, `Z`) for a gesture sequence.
   - Filters raw data for noise reduction and stability.
   - Displays live data on **Teleplot**.

2. **Gesture Recording**:
   - Press the "Record Key" to record a gesture.
   - Recorded data is stored in a file on the microcontroller (`gesture_data.txt`).

3. **Gesture Matching**:
   - Press the "Enter Key" to test a gesture.
   - Compares the test gesture to the saved gesture with configurable tolerances.

4. **Visual Indicators**:
   - **Green LED**: Successful unlock.
   - **Red LED**: Unlock failed.

5. **File Management**:
   - Gesture data is saved to a file.
   - Press the "Delete Key" to remove the saved gesture.

6. **Auto Lock**:
   - Automatically locks after 10 seconds of inactivity.

---

### **System Requirements**
- **Hardware**:
  - STM32F429ZI development board with an integrated accelerometer and gyroscope.
  - LEDs for visual feedback.
  - Buttons for recording and entering gestures.
- **Software**:
  - PlatformIO with the Mbed framework.
  - Teleplot for live data visualization.

---

### **Setup Instructions**
1. Clone this repository:
   ```bash
   git clone https://github.com/VeronicaGupta/Sentry.git
   cd Sentry
   ```

2. Install dependencies:
   - Ensure PlatformIO is installed in your VSCode environment.
   - Open the project folder in PlatformIO.

3. Configure the `platformio.ini` file:
   ```ini
   [env:disco_f429zi]
   platform = ststm32
   board = disco_f429zi
   framework = mbed

   build_flags = 
       -D MBED_CONF_PLATFORM_STDIO_CONVERT_NEWLINES=1
       -D MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_FLOATING_POINT=1
   monitor_speed = 115200
   ```

4. Flash the firmware:
   - Connect the STM32 board to your computer.
   - Build and upload the code using the **PlatformIO Build** and **Upload** buttons.

5. Launch **Teleplot** for live data visualization:
   - Configure Teleplot to connect to the serial COM port at `9600` baud.

---

### **Usage Instructions**
1. **Recording a Gesture**:
   - Press the "Record Key" (e.g., `USER_BUTTON`) to start recording.
   - Perform the desired gesture while holding the board in your hand.
   - The system records for 3 seconds and saves the gesture.

2. **Testing a Gesture**:
   - Press the "Enter Key" to test a gesture.
   - Perform the gesture and wait for feedback:
     - **Green LED**: Unlock successful.
     - **Red LED**: Unlock failed.

3. **Deleting a Gesture**:
   - Press the "Delete Key" to delete the saved gesture.

4. **Auto Lock**:
   - The system locks automatically after 10 seconds of inactivity.

---

### **Code Structure**
```plaintext
src/
│   main.cpp           # Main application code
include/
│   gesture.h          # Gesture processing functions
│   file_io.h          # File handling for gesture data
│   imu.h              # IMU setup and data collection
```

---

### **Key Algorithms**
1. **Gesture Filtering**:
   - Implements a low-pass filter to reduce noise and stabilize gyroscope readings.

2. **Gesture Matching**:
   - Compares the saved gesture to the test gesture using element-wise tolerances.

3. **File Handling**:
   - Saves and loads gestures from `gesture_data.txt`.

4. **Event Handling**:
   - Interrupt-driven design for buttons.

---

### **Grading Criteria**
| **Criteria**                      | **Details**                                  | **Weight** |
|------------------------------------|----------------------------------------------|------------|
| **Objectives Achieved**            | Functional lock/unlock system with gestures  | **40%**    |
| **Repeatability and Robustness**   | Consistent gesture matching                  | **20%**    |
| **Ease of Use**                    | Intuitive interaction with the system        | **10%**    |
| **Creativity**                     | Innovative design elements                   | **10%**    |
| **Well Written Code**              | Clean, modular, and documented code          | **10%**    |
| **Complexity**                     | Advanced algorithms or features              | **10%**    |

---
