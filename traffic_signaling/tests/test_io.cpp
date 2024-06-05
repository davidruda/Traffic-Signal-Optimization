#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "simulation/simulation.hpp"

using namespace std::string_literals; // for string operator""s

void assert_equal(unsigned long a, unsigned long b, std::string_view msg = "") {
    if (a != b) {
        std::cout << msg << "\n";
        throw std::runtime_error{msg.data()};
    }
}

void test_io(const std::vector<std::string> &args, bool same_instance = false) {
    auto &&input_file = args[0];
    auto &&plan_file = args[1];

    city_plan::CityPlan city_plan{input_file};
    simulation::Simulation simulation{city_plan};
    for (auto &&schedule_option: {"default"s, "adaptive"s}) {
        if (schedule_option == "default") {
            simulation.default_schedules();
        }
        else if (schedule_option == "adaptive") {
            simulation.adaptive_schedules();
        }

        simulation.save_schedules(plan_file);
        auto score = simulation.score();

        unsigned long score_1;
        if (same_instance) {
            simulation.load_schedules(plan_file);
            score_1 = simulation.score();
        }
        else {
            simulation::Simulation simulation_1{city_plan};
            simulation_1.load_schedules(plan_file);
            score_1 = simulation_1.score();
        }

        assert_equal(
            score, score_1,
            "------------------------------- DATA "
            // Ad hoc way to get the data name from the input file name.
            + input_file.substr(input_file.find(".txt") - 1, 1)
            + " -------------------------------\n"
            + (same_instance ? "[test_io_same_instance] " : "[test_io] ") + "*"s + schedule_option + "* "
            + "Score mismatch: " + std::to_string(score) + " != " + std::to_string(score_1)
        );
    }
}

int main(int argc, char *argv[]) {
    std::vector<std::string> args{argv + 1, argv + argc};

    test_io(args);
    test_io(args, true);
}