#ifndef GESTURE_RECOGNIZER_H
#define GESTURE_RECOGNIZER_H

#include "gesture_recorder.h"
#include <stdbool.h>

bool recognize_gesture(const GestureData* reference, const GestureData* test, float threshold);

#endif
