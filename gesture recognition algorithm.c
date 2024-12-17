bool recognize_gesture(const std::vector<SensorData>& saved, const std::vector<SensorData>& test, float threshold) {
    int match_count = 0;

    for (size_t i = 0; i < saved.size() && i < test.size(); i++) {
        float diff = fabs(saved[i].Gx - test[i].Gx) +
                     fabs(saved[i].Gy - test[i].Gy) +
                     fabs(saved[i].Gz - test[i].Gz) +
                     fabs(saved[i].Ax - test[i].Ax) +
                     fabs(saved[i].Ay - test[i].Ay) +
                     fabs(saved[i].Az - test[i].Az);
        if (diff < threshold) {
            match_count++;
        }
    }

    return (float)match_count / saved.size() > 0.8; // 80% match
}
