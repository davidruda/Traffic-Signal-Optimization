#include "city_plan/car.hpp"

namespace city_plan {

    std::ostream &operator<<(std::ostream &os, const Car &obj) {
        os << "[" << obj.id_ << " path_length: " << obj.path_.size() << "]";
        return os;
    }

}
