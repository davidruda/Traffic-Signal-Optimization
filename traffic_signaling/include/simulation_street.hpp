#ifndef SIMULATION_STREET_HPP
#define SIMULATION_STREET_HPP

#include <optional>
#include <queue>
#include <string>

#include "city_plan.hpp"

namespace simulation {

    class Street {
    public:
        explicit Street(const city_plan::Street &data);

        size_t car_queue_size() const;
        void add_car(size_t car_id);
        size_t get_car(size_t time);

        size_t id() const;
        size_t end() const;
        const std::string &name() const;
        size_t length() const;

        std::optional<size_t> last_used_time() const;
        bool is_used() const;
        void reset();

    private:
        const city_plan::Street &data_;

        // car queue represented by car ids
        std::queue<size_t> car_queue_;
        std::optional<size_t> last_used_time_;
    };

}// namespace simulation

#endif
