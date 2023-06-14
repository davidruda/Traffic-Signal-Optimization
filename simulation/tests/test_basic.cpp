#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "simulation.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> args{argv + 1, argv + argc};
    auto &&input_file = args[0];

    Simulation simulation{input_file};
    auto &&simulation_instance = simulation.create_instance();
    simulation_instance.create_plan_default();

    auto score = simulation_instance.run().score(true);
    std::cout << score << "\n";
}
