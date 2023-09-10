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

    size_t id() const {
        return data_.id();
    }

    size_t end() const {
        return data_.end();
    }

    const std::string &name() const {
        return data_.name();
    }

    size_t length() const {
        return data_.length();
    }

    bool used() const {
        return data_.used();
    }

    std::optional<size_t> latest_used_time() const {
        return latest_used_time_;
    }

    void add_car(size_t car_id, size_t time);
    size_t get_car();
    void reset();

private:
    const city_plan::Street &data_;

    // car queue represented by car ids
    std::queue<size_t> car_queue_;
    std::optional<size_t> latest_used_time_;
};
}

#endif
