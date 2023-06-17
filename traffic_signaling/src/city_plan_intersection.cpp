#include <iostream>

#include "city_plan_intersection.hpp"

namespace city_plan {

    Intersection::Intersection(size_t id) : id_(id) {}


    void Intersection::add_incoming(size_t street_id) {
        incoming_streets_.emplace_back(street_id);
    }

    size_t Intersection::id() const {
        return id_;
    }

    const std::vector<size_t> &Intersection::incoming() const {
        return incoming_streets_;
    }

    std::ostream &operator<<(std::ostream &os, const Intersection &obj) {
        os << obj.id_ << "\n"
           << "Incoming streets:\n";
        for (auto &&s: obj.incoming_streets_) {
            os << s << "\n";
        }
        return os;
    }

}
