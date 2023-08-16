#include "simulation/car.hpp"

namespace simulation {

    void Car::reset() {
        path_index_ = START;
        finish_time_ = UNFINISHED;
    }

}
