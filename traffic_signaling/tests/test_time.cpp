#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "simulation/simulation.hpp"

#define TIME(statement, name)                          \
    start = std::chrono::high_resolution_clock::now(); \
    statement;                                         \
    end = std::chrono::high_resolution_clock::now();   \
    std::cout << name << ": " << std::chrono::duration<double>{end - start}.count() << "s\n";

int main(int argc, char *argv[]) {
    std::vector<std::string> args{argv + 1, argv + argc};
    auto &&input_file = args[0];
    auto &&plan_file = args[1];

    std::cout << "------------------------------- DATA "
              // Ad hoc way to get the data name from the input file name.
              << input_file.substr(input_file.find(".txt") - 1, 1)
              << " -------------------------------\n";

    std::chrono::high_resolution_clock::time_point start, end;

    TIME(city_plan::CityPlan city_plan{input_file}, "CityPlan constructor")
    TIME(simulation::Simulation simulation{city_plan}, "Simulation constructor")
    TIME(simulation.default_schedules(), "default_schedules")
    TIME(simulation.write_schedules(plan_file), "write_schedules")
    TIME(simulation.read_schedules(plan_file), "read_schedules")
    TIME(auto score = simulation.score(), "score")

    std::cout << score << "\n";
}
