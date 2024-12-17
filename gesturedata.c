#include "mbed.h"
#include "nlohmann/json.hpp" // JSON library for Mbed
#include <vector>
#include <fstream>

using json = nlohmann::json;
using namespace std;

std::vector<SensorData> record_gesture(int duration) {
    Timer timer;
    std::vector<SensorData> data;

    timer.start();
    while (timer.read() < duration) {
        data.push_back(read_sensor_data());
        thread_sleep_for(100);
    }
    timer.stop();

    // Save data to JSON file
    json j_data = data;
    std::ofstream file("/local/gesture.json");
    file << j_data.dump(4);
    file.close();

    return data;
}

std::vector<SensorData> load_gesture_data() {
    std::ifstream file("/local/gesture.json");
    json j_data;
    file >> j_data;
    file.close();

    return j_data.get<std::vector<SensorData>>();
}
