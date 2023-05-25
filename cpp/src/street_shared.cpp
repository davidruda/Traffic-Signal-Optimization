
#include <iostream>

#include "street_shared.hpp"

StreetShared::StreetShared(
        int id,
        IntersectionShared &start,
        IntersectionShared &end,
        std::string name,
        int length) : id_(id), start_(start), end_(end), name_(std::move(name)), length_(length), used_(false) {}

int StreetShared::id() const {
    return id_;
}

IntersectionShared &StreetShared::start() const {
    return start_;
}

IntersectionShared &StreetShared::end() const {
    return end_;
}

const std::string &StreetShared::name() const {
    return name_;
}

int StreetShared::length() const {
    return length_;
}

bool StreetShared::is_used() const {
    return used_;
}

void StreetShared::set_used(bool used) {
    used_ = used;
}

std::ostream &operator<<(std::ostream &os, const StreetShared &obj) {
    os << "[" << obj.id_ << " " << obj.name_ << " " << obj.start_.id()
       << " -> " << obj.end_.id() << " length: " << obj.length_ << "]";
    return os;
}
