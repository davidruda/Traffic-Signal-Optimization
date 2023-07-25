#include <iostream>

#include "city_plan_intersection.hpp"

namespace city_plan {

    std::ostream &operator<<(std::ostream &os, const Intersection &obj) {
        os << obj.id_ << "\n"
           << "Incoming streets:\n";
        for (auto &&s: obj.incoming_streets_) {
            os << s << "\n";
        }
        return os;
    }

}
