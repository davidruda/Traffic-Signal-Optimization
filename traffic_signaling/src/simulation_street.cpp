#include "simulation_street.hpp"

namespace simulation {

    Street::Street(const city_plan::Street &data) : data_(data) {}

    void Street::add_car(size_t car_id) {
        car_queue_.emplace(car_id);
    }

    size_t Street::car_queue_size() const {
        return car_queue_.size();
    }

    size_t Street::get_car(size_t time) {
        auto &&car = car_queue_.front();
        car_queue_.pop();
        last_used_time_ = time;
        return car;
    }

    size_t Street::id() const {
        return data_.id();
    }

    size_t Street::end() const {
        return data_.end();
    }

    const std::string &Street::name() const {
        return data_.name();
    }

    size_t Street::length() const {
        return data_.length();
    }

    std::optional<size_t> Street::last_used_time() const {
        return last_used_time_;
    }

    bool Street::is_used() const {
        return data_.is_used();
    }

    void Street::reset() {
        car_queue_ = {};
        last_used_time_ = std::nullopt;
    }

}// namespace simulation
