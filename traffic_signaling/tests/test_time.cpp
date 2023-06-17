#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "simulation.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> args{argv + 1, argv + argc};
    auto &&input_file = args[0];
    auto &&plan_file = args[1];

    auto start = std::chrono::high_resolution_clock::now();
    city_plan::CityPlan city_plan{input_file};
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "CityPlan constructor: " << std::chrono::duration<double>{end - start}.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    simulation::Simulation simulation{city_plan};
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Simulation constructor: " << std::chrono::duration<double>{end - start}.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    simulation.create_plan_default();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "create_plan_default: " << std::chrono::duration<double>{end - start}.count() << "s\n";

    // start = std::chrono::high_resolution_clock::now();
    // simulation.read_plan(plan_file);
    // end = std::chrono::high_resolution_clock::now();
    // std::cout << "read_plan: " << std::chrono::duration<double>{end - start}.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    simulation.run();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "run: " << std::chrono::duration<double>{end - start}.count() << "s\n";
}
