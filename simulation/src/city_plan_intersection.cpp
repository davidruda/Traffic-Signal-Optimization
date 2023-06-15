#include <iostream>

#include "city_plan_intersection.hpp"

CityPlanIntersection::CityPlanIntersection(size_t id) : id_(id) {}


void CityPlanIntersection::add_incoming(size_t street_id) {
    incoming_streets_.emplace_back(street_id);
}

size_t CityPlanIntersection::id() const {
    return id_;
}

const std::vector<size_t> &CityPlanIntersection::incoming() const {
    return incoming_streets_;
}

std::ostream &operator<<(std::ostream &os, const CityPlanIntersection &obj) {
    os << obj.id_ << "\n"
       << "Incoming streets:\n";
    for (auto &&s: obj.incoming_streets_) {
        os << s << "\n";
    }
    return os;
}
