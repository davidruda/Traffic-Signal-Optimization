#include "simulation_intersection.hpp"

namespace simulation {

    Intersection::Intersection(const city_plan::Intersection &data)
        : data_(data) {}

    size_t Intersection::id() const {
        return data_.id();
    }

    const std::vector<size_t> &Intersection::incoming() const {
        return data_.incoming();
    }

    void Intersection::reset() {}

}// namespace simulation
