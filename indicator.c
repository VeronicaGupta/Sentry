#include "indicator.h"

void indicateSuccess(void) {
    for (int i = 0; i < 3; i++) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);
    }
}

void indicateFailure(void) {
    for (int i = 0; i < 3; i++) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);
        HAL_Delay(500);
    }
}

