#ifndef SIMULATION_INTERSECTION_HPP
#define SIMULATION_INTERSECTION_HPP

#include <vector>

#include "city_plan.hpp"

class SimulationIntersection {
public:
    explicit SimulationIntersection(const CityPlanIntersection &data);

    size_t id() const;
    const std::vector<size_t> &incoming() const;
    void reset();

private:
    const CityPlanIntersection &data_;
};


#endif
