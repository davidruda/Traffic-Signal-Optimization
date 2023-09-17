#ifndef SIMULATION_STREET_HPP
#define SIMULATION_STREET_HPP

#include <optional>
#include <queue>
#include <string>

#include "city_plan/city_plan.hpp"

namespace simulation {
class Street {
public:
    explicit Street(const city_plan::Street &data)
        : data_(data) {}

    void add_car(size_t car_id, size_t time) {
        car_queue_.emplace(car_id);
        latest_used_time_ = time;
    }

    size_t get_car() {
        auto &&car = car_queue_.front();
        car_queue_.pop();
        return car;
    }

    size_t id() const {
        return data_.id();
    }

    std::optional<size_t> latest_used_time() const {
        return latest_used_time_;
    }

    void reset() {
        car_queue_ = {};
        latest_used_time_ = {};
    }

private:
    const city_plan::Street &data_;

    // car queue represented by car ids
    std::queue<size_t> car_queue_;
    std::optional<size_t> latest_used_time_;
};
}

#endif
