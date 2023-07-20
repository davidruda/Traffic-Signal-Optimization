#ifndef SIMULATION_CAR_HPP
#define SIMULATION_CAR_HPP

#include <functional>
#include <limits>
#include <utility>
#include <vector>

#include "city_plan.hpp"

namespace simulation {

    class Car {
    public:
        explicit Car(const city_plan::Car &data);

        size_t current_street() const;
        void move_to_next_street();
        bool final_destination() const;

        size_t id() const;

        void set_finish_time(size_t finish_time);
        size_t finish_time() const;
        bool finished() const;
        void reset();

    private:
        const city_plan::Car &data_;

        size_t path_index_;
        size_t finish_time_;

        const static size_t START{0};
        const static size_t UNFINISHED{std::numeric_limits<size_t>::max()};
    };

}

#endif
