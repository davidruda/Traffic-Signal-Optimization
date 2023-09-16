#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "simulation/simulation.hpp"

void test_io(const std::vector<std::string> &args, bool same_instance = false) {
    auto &&input_file = args[0];
    auto &&plan_file = args[1];

    city_plan::CityPlan city_plan{input_file};
    simulation::Simulation simulation{city_plan};
    simulation.default_schedules();
    simulation.write_schedules(plan_file);
    auto score = simulation.score();

    size_t score_1;
    if (same_instance) {
        simulation.read_schedules(plan_file);
        score_1 = simulation.score();
    }
    else {
        simulation::Simulation simulation_1{city_plan};
        simulation_1.read_schedules(plan_file);
        score_1 = simulation_1.score();
    }

    if (score != score_1) {
        std::cout
            << "------------------------------- DATA "
            // Ad hoc way to get the data name from the input file name.
            << input_file.substr(input_file.find(".txt") - 1, 1)
            << " -------------------------------\n";

        auto &&msg =
            "Score mismatch: " + std::to_string(score) + " != " +
            std::to_string(score_1);
        msg = (same_instance ? "[test_io_same_instance] " : "[test_io] ") + msg;
        std::cout << msg << "\n";
        throw std::runtime_error(msg);
    }
}


int main(int argc, char *argv[]) {
    std::vector<std::string> args{argv + 1, argv + argc};

    test_io(args);
    test_io(args, true);
}