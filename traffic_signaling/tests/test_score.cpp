#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <string_view>

#include "simulation/simulation.hpp"

using namespace std::string_literals; // for string operator""s

static const std::unordered_map<std::string_view, unsigned long> DEFAULT_SCORE{
    {"a", 1'001},
    {"b", 4'566'576},
    {"c", 1'299'357},
    {"d", 1'573'100},
    {"e", 684'769},
    {"f", 819'083}
};

static const std::unordered_map<std::string_view, unsigned long> ADAPTIVE_SCORE{
    {"a", 2'002},
    {"b", 4'568'568},
    {"c", 1'305'874},
    {"d", 2'477'913},
    {"e", 702'570},
    {"f", 824'879}
};

// Theoretical maximum score if none of the cars ever has to wait at a traffic light.
static const std::unordered_map<std::string_view, unsigned long> UPPER_BOUND{
    {"a", 2'002},
    {"b", 4'576'202},
    {"c", 1'328'389},
    {"d", 3'986'591},
    {"e", 921'203},
    {"f", 1'765'068}
};

void assert_equal(unsigned long a, unsigned long b, std::string_view msg = "") {
    if (a != b) {
        std::cout << msg << "\n";
        throw std::runtime_error{msg.data()};
    }
}

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
    auto &&schedule_option = {"default"s, "adaptive"s, "random"s, "scaled"s};
    for (auto &&option: schedule_option) {
        unsigned long expected{};
        if (option == "default") {
            simulation.default_schedules();
            expected = DEFAULT_SCORE.at(data);
            std::cout
                << "************************* default_schedules "
                   "**************************\n";
        }
        else if (option == "adaptive") {
            simulation.adaptive_schedules();
            expected = ADAPTIVE_SCORE.at(data);
            std::cout 
                << "\n************************* adaptive_schedules "
                   "*************************\n";
        }
        else if (option == "random") {
            simulation::set_seed(42);
            simulation.random_schedules();
            expected = simulation.score();
            simulation::set_seed(42);
            simulation.random_schedules();
            std::cout
                << "\n************************** random_schedules "
                   "**************************\n";
        }
        else if (option == "scaled") {
            simulation.scaled_schedules(27);
            expected = simulation.score();
            std::cout
                << "\n************************** scaled_schedules "
                   "**************************\n";
        }

        auto score = simulation.score();
        simulation.summary();
        assert_equal(
            score, expected,
            "[" + option + "] Score mismatch: " + std::to_string(score)
            + " != " + std::to_string(expected)
        );
    }

    auto score = city_plan.upper_bound();
    auto expected = UPPER_BOUND.at(data);
    assert_equal(
        score, expected,
        "[upper_bound] Score mismatch: " + std::to_string(score)
        + " != " + std::to_string(expected)
    );
}
