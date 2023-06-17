#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "simulation.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> args{argv + 1, argv + argc};
    auto &&input_file = args[0];

    city_plan::CityPlan city_plan{input_file};
    simulation::Simulation simulation{city_plan};
    simulation.create_plan_default();

    simulation.run().summary();
    std::cout << simulation.score() << "\n";
}
