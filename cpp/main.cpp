#include <chrono>
#include <iostream>

#include "simulation.hpp"

int main(int argc, char *argv[]) {
    auto input_file = argv[1];
    auto plan_file = argv[2];
    //auto input_file = "E:/Rocnikovy_projekt/david_ruda/input_data/a_an_example.in.txt";
    //auto plan_file = "E:/Rocnikovy_projekt/david_ruda/output/a_an_example.out.txt";
    //auto input_file = "/mnt/e/Rocnikovy_projekt/david_ruda/input_data/a_an_example.in.txt";
    //auto plan_file = "/mnt/e/Rocnikovy_projekt/david_ruda/output/a_an_example.out.txt";

    Simulation simulation{input_file};
    auto &&simulation_instance = simulation.create_instance();
    simulation_instance.read_plan(plan_file);

    //simulation_instance.write_plan("test_file_plan.txt");
    //simulation_instance.read_plan("test_file_plan.txt");

    auto score = simulation_instance.run().score(true);
    std::cout << score << "\n";
}
