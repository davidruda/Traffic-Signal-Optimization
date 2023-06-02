
#include <iostream>

#include "intersection.hpp"

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

const std::vector<std::reference_wrapper<const StreetShared>> &IntersectionShared::incoming() const {
    return incoming_;
}

std::ostream &operator<<(std::ostream &os, const IntersectionShared &obj) {
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

IntersectionInstance::IntersectionInstance(const IntersectionShared &data) : data_(data) {}

const IntersectionShared &IntersectionInstance::data() const {
    return data_;
}
