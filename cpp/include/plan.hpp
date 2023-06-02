#ifndef PLAN_HPP
#define PLAN_HPP


#include <ranges>
#include <string>
#include <unordered_map>

#include "simulation.hpp"

class PlanShared {
};

class PlanInstance {
public:
    explicit PlanInstance(const SimulationInstance &simulation, const std::string &filename);
    //explicit PlanInstance(const PlanShared &data);

    void create_default(const SimulationInstance &simulation, bool used);

private:
    const SimulationInstance &simulation_;
    //const PlanShared &data_;

    std::unordered_map<int, std::vector<std::ranges::iota_view<size_t>>> plan_;
};


#endif
