#include <iostream>

#include "simulation_intersection.hpp"

SimulationIntersection::SimulationIntersection(const CityPlanIntersection &data)
    : data_(data) {}

size_t SimulationIntersection::id() const {
    return data_.id();
}

const std::vector<size_t> &SimulationIntersection::incoming() const {
    return data_.incoming();
}

void SimulationIntersection::reset() {}