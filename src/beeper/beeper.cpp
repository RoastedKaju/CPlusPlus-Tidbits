#include <iostream>
#include <chrono>

int main() {
    const auto current_time = std::chrono::system_clock::now();
    const std::chrono::duration<double> beep_interval{3.0};

    // next beep time
    auto next_beep = current_time + beep_interval;

    // beep counter
    uint32_t beep_count{0};

    while (beep_count < 10) {
        if (std::chrono::system_clock::now() > next_beep) {
            std::cout << "Beeped at " << std::chrono::system_clock::now() << '\n';
            next_beep = std::chrono::system_clock::now() + beep_interval;
            ++beep_count;
        }
    }

    std::cout << "Program ended" << std::endl;
    return EXIT_SUCCESS;
}
