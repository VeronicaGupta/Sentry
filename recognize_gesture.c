bool recognize_gesture(const std::vector<SensorData>& reference_gesture, 
                       const std::vector<SensorData>& test_gesture, 
                       float threshold) {
    // Validate gesture lengths
    if (reference_gesture.size() != test_gesture.size()) {
        printf("Error: Gestures do not have the same number of samples.\n");
        return false;
    }

    float total_distance = 0.0f;

    // Compute Euclidean distance for each sample
    for (size_t i = 0; i < reference_gesture.size(); ++i) {
        float distance = sqrt(
            pow(reference_gesture[i].Gx - test_gesture[i].Gx, 2) +
            pow(reference_gesture[i].Gy - test_gesture[i].Gy, 2) +
            pow(reference_gesture[i].Gz - test_gesture[i].Gz, 2) +
            pow(reference_gesture[i].Ax - test_gesture[i].Ax, 2) +
            pow(reference_gesture[i].Ay - test_gesture[i].Ay, 2) +
            pow(reference_gesture[i].Az - test_gesture[i].Az, 2)
        );
        total_distance += distance;
    }

    // Calculate average distance
    float average_distance = total_distance / reference_gesture.size();

    // Debugging output
    printf("Average Distance: %.2f\n", average_distance);

    // Compare against threshold
    return (average_distance <= threshold);
}
