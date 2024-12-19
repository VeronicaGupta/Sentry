std::vector<SensorData> reference_gesture = load_gesture_data(); // Load from JSON
std::vector<SensorData> test_gesture;
record_gesture(3, test_gesture); // Record new gesture

bool match = recognize_gesture(reference_gesture, test_gesture, 0.5); // Use threshold = 0.5
if (match) {
    printf("Gesture Recognized: Unlock Successful\n");
    green_led_on();
} else {
    printf("Gesture Not Recognized: Unlock Failed\n");
    red_led_on();
}
