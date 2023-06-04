#include <chrono>
#include <iostream>

#include "simulation.hpp"

int main(int argc, char *argv[]) {
    auto input_file = argv[1];
    auto plan_file = argv[2];

    auto start = std::chrono::high_resolution_clock::now();
    Simulation simulation{input_file};
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Simulation constructor: " << std::chrono::duration<double>{end - start}.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    auto &&simulation_instance = simulation.create_instance();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "create_instance: " << std::chrono::duration<double>{end - start}.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    simulation_instance.read_plan(plan_file);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "read_plan: " << std::chrono::duration<double>{end - start}.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    simulation_instance.run();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "run: " << std::chrono::duration<double>{end - start}.count() << "s\n";
}
