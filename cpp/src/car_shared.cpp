
#include <iostream>

#include "car_shared.hpp"

CarShared::CarShared(
        int id,
        int path_length,
        std::vector<std::reference_wrapper<const StreetShared>> path
        ) : id_(id), path_length_(path_length), path_(std::move(path)) {}

std::ostream &operator<<(std::ostream &os, const CarShared &obj) {
    os << "[" << obj.id_ << " path_length: " << obj.path_length_ << "]";
    return os;
}
