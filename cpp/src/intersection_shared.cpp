
#include <iostream>

#include "intersection_shared.hpp"

IntersectionShared::IntersectionShared(int id) : id_(id) {}

int IntersectionShared::id() const {
    return id_;
}

void IntersectionShared::add_incoming(const StreetShared &street) {
    incoming_.emplace_back(std::cref(street));
}

void IntersectionShared::add_outgoing(const StreetShared &street) {
    outgoing_.emplace_back(std::cref(street));
}

std::ostream &operator<<(std::ostream &os, const IntersectionShared &obj) {
    os << obj.id_ << "\n" << "Incoming streets:\n";
    for (auto&& s:obj.incoming_) {
        os << s.get() << "\n";
    }
    os << "Outgoing streets:\n";
    for (auto&& s:obj.outgoing_) {
        os << s.get() << "\n";
    }
    return os;
}
