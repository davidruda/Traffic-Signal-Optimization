#ifndef SIMULATION_INTERSECTION_HPP
#define SIMULATION_INTERSECTION_HPP

#include <vector>

#include "city_plan/city_plan.hpp"

namespace simulation {

    class Intersection {
    public:
        explicit Intersection(const city_plan::Intersection &data)
            : data_(data) {}

        size_t id() const {
            return data_.id();
        }

        const std::vector<size_t> &incoming() const {
            return data_.incoming();
        }

        // Does nothing - Keep this method for consistency
        void reset() {}

    private:
        const city_plan::Intersection &data_;
    };

}

#endif
