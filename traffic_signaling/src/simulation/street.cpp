#include "simulation/street.hpp"

namespace simulation {

    void Street::add_car(size_t car_id, size_t time) {
        car_queue_.emplace(car_id);
        latest_used_time_ = time;
    }

    size_t Street::get_car() {
        auto &&car = car_queue_.front();
        car_queue_.pop();
        return car;
    }

    void Street::reset() {
        car_queue_ = {};
        latest_used_time_ = {};
    }
}
