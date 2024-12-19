#include "gesture_recorder.h"
#include "gesture_recognizer.h"
#include "indicator.h"
#include "stm32f4xx_hal.h"

DigitalOut greenLED(GPIO_PIN_13, GPIOC);  // Onboard LED
DigitalOut redLED(GPIO_PIN_14, GPIOC);
DigitalIn recordButton(GPIO_PIN_0, GPIOA);
DigitalIn unlockButton(GPIO_PIN_1, GPIOA);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();

    MPU6050_Init(&hi2c1);

    while (1) {
        if (recordButton == GPIO_PIN_SET) {
            GestureData reference_gesture = record_gesture(3);
            save_gesture(&reference_gesture);
            indicateSuccess();
        }

        if (unlockButton == GPIO_PIN_SET) {
            GestureData saved_gesture = load_gesture();
            GestureData test_gesture = record_gesture(3);
            if (recognize_gesture(&saved_gesture, &test_gesture, 0.5)) {
                indicateSuccess();
            } else {
                indicateFailure();
            }
        }
    }
}
