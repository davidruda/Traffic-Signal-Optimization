#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "simulation/simulation.hpp"

std::unordered_map<std::string, size_t> SCORES = {
    {"a", 1001},
    {"b", 4566576},
    {"c", 1299357},
    {"d", 1573100},
    {"e", 684769},
    {"f", 819083}
};

int main(int argc, char *argv[]) {
    std::vector<std::string> args{argv + 1, argv + argc};
    auto &&input_file = args[0];

    // Ad hoc way to get the data name from the input file name.
    auto data = input_file.substr(input_file.find(".txt") - 1, 1);
    std::cout
        << "------------------------------- DATA " << data
        << " -------------------------------\n";

    city_plan::CityPlan city_plan{input_file};
    simulation::Simulation simulation{city_plan};
    simulation.default_schedules();
    auto score = simulation.score();
    simulation.summary();

    if (score != SCORES[data]) {
        auto &&msg =
            "Score mismatch: " + std::to_string(score) + " != " +
            std::to_string(SCORES[data]);
        std::cout << msg << "\n";
        throw std::runtime_error(msg);
    }
}
