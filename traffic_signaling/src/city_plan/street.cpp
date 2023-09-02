#include "city_plan/street.hpp"

namespace city_plan {

    std::ostream &operator<<(std::ostream &os, const Street &obj) {
        os << "[" << obj.id_ << " " << obj.name_ << " " << obj.start_id_
           << " -> " << obj.end_id_ << " length: " << obj.length_ << "]";
        return os;
    }

}
