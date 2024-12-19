#include "gesture_recognizer.h"
#include <math.h>

bool recognize_gesture(const GestureData* reference, const GestureData* test, float threshold) {
    if (reference->length != test->length) {
        return false;
    }

    float total_distance = 0.0f;

    for (size_t i = 0; i < reference->length; ++i) {
        float distance = sqrt(
            pow(reference->data[i].Gx - test->data[i].Gx, 2) +
            pow(reference->data[i].Gy - test->data[i].Gy, 2) +
            pow(reference->data[i].Gz - test->data[i].Gz, 2) +
            pow(reference->data[i].Ax - test->data[i].Ax, 2) +
            pow(reference->data[i].Ay - test->data[i].Ay, 2) +
            pow(reference->data[i].Az - test->data[i].Az, 2)
        );
        total_distance += distance;
    }

    float average_distance = total_distance / reference->length;
    return average_distance <= threshold;
}
