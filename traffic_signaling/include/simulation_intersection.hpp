#ifndef SIMULATION_INTERSECTION_HPP
#define SIMULATION_INTERSECTION_HPP

#include <vector>

#include "city_plan.hpp"

namespace simulation {

    class Intersection {
    public:
        explicit Intersection(const city_plan::Intersection &data);

        size_t id() const;
        const std::vector<size_t> &incoming() const;
        void reset();

    private:
        const city_plan::Intersection &data_;
    };

}

#endif
