#ifndef SIMULATION_INSTANCE_HPP
#define SIMULATION_INSTANCE_HPP

#include "simulation_shared.hpp"
#include "intersection_instance.hpp"
#include "street_instance.hpp"
#include "car_instance.hpp"

class SimulationInstance {
public:
    explicit SimulationInstance(const SimulationShared &data);

    void run();

private:
    const SimulationShared & data_;

    std::vector<IntersectionInstance> intersections_;
    std::vector<StreetInstance> streets_;
    std::vector<CarInstance> cars_;
};


#endif
