#include "simulation_street.hpp"

namespace simulation {

    size_t Street::get_car(size_t time) {
        auto &&car = car_queue_.front();
        car_queue_.pop();
        last_used_time_ = time;
        return car;
    }

    void Street::reset() {
        car_queue_ = {};
        last_used_time_ = std::nullopt;
    }

}
