
#include <iostream>

#include "intersection.hpp"

Intersection::Intersection(int id) : id_(id) {}

int Intersection::id() const {
    return id_;
}

void Intersection::add_incoming(const Street &street) {
    incoming_.emplace_back(std::cref(street));
}

void Intersection::add_outgoing(const Street &street) {
    outgoing_.emplace_back(std::cref(street));
}

const std::vector<std::reference_wrapper<const Street>> &Intersection::incoming() const {
    return incoming_;
}

std::ostream &operator<<(std::ostream &os, const Intersection &obj) {
    os << obj.id_ << "\n"
       << "Incoming streets:\n";
    for (auto &&s: obj.incoming_) {
        os << s.get() << "\n";
    }
    os << "Outgoing streets:\n";
    for (auto &&s: obj.outgoing_) {
        os << s.get() << "\n";
    }
    return os;
}

Intersection::Instance::Instance(const Intersection &data) : data_(data) {}

int Intersection::Instance::id() const {
    return data_.id_;
}

const std::vector<std::reference_wrapper<const Street>> &Intersection::Instance::incoming() const {
    return data_.incoming_;
}
