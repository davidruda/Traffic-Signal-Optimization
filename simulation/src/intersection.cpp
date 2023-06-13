
#include <iostream>

#include "intersection.hpp"

Intersection::Intersection(size_t id) : id_(id) {}

void Intersection::add_incoming(size_t street_id) {
    incoming_streets_.emplace_back(street_id);
}

std::ostream &operator<<(std::ostream &os, const Intersection &obj) {
    os << obj.id_ << "\n"
       << "Incoming streets:\n";
    for (auto &&s: obj.incoming_streets_) {
        os << s << "\n";
    }
    return os;
}

Intersection::Instance::Instance(const Intersection &data) : data_(data) {}

size_t Intersection::Instance::id() const {
    return data_.id_;
}

const std::vector<size_t> &Intersection::Instance::incoming() const {
    return data_.incoming_streets_;
}

void Intersection::Instance::reset() {}
