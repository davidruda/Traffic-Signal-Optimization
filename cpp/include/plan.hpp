#ifndef PLAN_HPP
#define PLAN_HPP


#include <ranges>
#include <string>
#include <unordered_map>

#include "simulation.hpp"

class Plan {
public:
    class Instance;
};

class Plan::Instance {
public:
    explicit Instance(const SimulationInstance &simulation, const std::string &filename);
    //explicit PlanInstance(const Plan &data);

    void create_default(const SimulationInstance &simulation, bool used);

private:
    const SimulationInstance &simulation_;
    //const Plan &data_;

    std::unordered_map<int, std::vector<std::ranges::iota_view<size_t>>> plan_;
};


#endif
