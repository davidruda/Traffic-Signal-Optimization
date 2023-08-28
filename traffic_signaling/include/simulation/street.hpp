#ifndef SIMULATION_STREET_HPP
#define SIMULATION_STREET_HPP

#include <queue>
#include <string>

#include "city_plan/city_plan.hpp"

namespace simulation {

    class Street {
    public:
        explicit Street(const city_plan::Street &data) : data_(data) {}

        size_t car_queue_size() const {
            return car_queue_.size();
        }

        void add_car(size_t car_id) {
            car_queue_.emplace(car_id);
        }

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

        bool is_used() const {
            return data_.is_used();
        }

        void reset() {
            car_queue_ = {};
        }

        size_t get_car();

    private:
        const city_plan::Street &data_;

        // car queue represented by car ids
        std::queue<size_t> car_queue_;
    };

}

#endif
