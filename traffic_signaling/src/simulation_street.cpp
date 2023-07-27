#include "simulation_street.hpp"

namespace simulation {

    size_t Street::get_car() {
        auto &&car = car_queue_.front();
        car_queue_.pop();
        return car;
    }

}
