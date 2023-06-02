#include <iostream>

#include "simulation.hpp"

int main() {
    auto filename = "E:/Rocnikovy_projekt/david_ruda/input_data/a_an_example.in.txt";
    SimulationShared sim{filename};
    std::cout << "\n"
              << sim;
}
