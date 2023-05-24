#ifndef SIMULATION_INSTANCE_HPP
#define SIMULATION_INSTANCE_HPP

#include "simulation_shared.hpp"
#include "intersection_instance.hpp"
#include "street_instance.hpp"
#include "car_instance.hpp"

class SimulationInstance {
private:

public:
    const SimulationShared* const data;

    std::vector<IntersectionInstance> intersections;
    std::vector<StreetInstance> streets;
    std::vector<CarInstance> cars;
};


#endif
