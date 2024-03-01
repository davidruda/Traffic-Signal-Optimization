#ifndef SIMULATION_STREET_HPP
#define SIMULATION_STREET_HPP

#include <optional>
#include <queue>

#include "city_plan/street.hpp"

namespace simulation {
class Street {
public:
    explicit Street(const city_plan::Street &data)
        : data_(data) {}

    void add_car(unsigned long car_id, unsigned long time) {
        car_queue_.emplace(car_id);
        latest_used_time_ = time;
    }

    unsigned long get_car() {
        auto &&car = car_queue_.front();
        car_queue_.pop();
        return car;
    }

    unsigned long id() const {
        return data_.id();
    }

    std::optional<unsigned long> latest_used_time() const {
        return latest_used_time_;
    }

    void reset() {
        car_queue_ = {};
        latest_used_time_ = {};
    }

private:
    const city_plan::Street &data_;

    // car queue represented by car ids
    std::queue<unsigned long> car_queue_;
    std::optional<unsigned long> latest_used_time_;
};
}

#endif
