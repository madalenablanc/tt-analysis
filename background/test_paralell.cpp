#include <iostream>
#include <vector>
#include <cmath>

// Define the processing function outside of main
void processEvent(const std::vector<int>& data, int threshold) {
    for (const auto& value : data) {
        if (value > threshold) {
            std::cout << "Value " << value << " exceeds threshold." << std::endl;
        }
    }
}

int main() {
    std::vector<int> eventData = {10, 20, 30, 40, 50};
    int threshold = 25;

    // Call the processing function from main
    processEvent(eventData, threshold);

    return 0;
}
