#ifndef SIMULATION_CAR_HPP
#define SIMULATION_CAR_HPP

#include <functional>
#include <limits>
#include <utility>
#include <vector>

#include "city_plan/city_plan.hpp"

namespace simulation {

    class Car {
    public:
        explicit Car(const city_plan::Car &data) : data_(data) {}

        size_t current_street() const {
            return data_.path()[path_index_];
        }

        void move_to_next_street() {
            ++path_index_;
        }

        bool final_destination() const {
            return data_.path().size() - 1 == path_index_;
        }

        size_t id() const {
            return data_.id();
        }

        void set_finish_time(size_t finish_time) {
            finish_time_ = finish_time;
        }

        size_t finish_time() const {
            return finish_time_;
        }

        bool finished() const {
            return finish_time_ != UNFINISHED;
        }

        void reset();

    private:
        const city_plan::Car &data_;

        static constexpr size_t START{};
        static constexpr size_t UNFINISHED{std::numeric_limits<size_t>::max()};

        size_t path_index_{START};
        size_t finish_time_{UNFINISHED};
    };

}

#endif
