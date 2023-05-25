
#include <iostream>

#include "car_shared.hpp"

CarShared::CarShared(
        int id,
        int path_length,
        std::vector<std::reference_wrapper<const StreetShared>> path
        ) : id_(id), path_length_(path_length), path_(std::move(path)) {}

int CarShared::id() const {
    return id_;
}

int CarShared::path_length() const {
    return path_length_;
}

const std::vector<std::reference_wrapper<const StreetShared>> &CarShared::path() const {
    return path_;
}

std::ostream &operator<<(std::ostream &os, const CarShared &obj) {
    os << "[" << obj.id_ << " path_length: " << obj.path_length_ << "]";
    return os;
}
