#include <iostream>

#include "city_plan_car.hpp"

namespace city_plan {

    Car::Car(size_t id, std::vector<size_t> path) : id_(id), path_(std::move(path)) {}

    size_t Car::id() const {
        return id_;
    }

    const std::vector<size_t> &Car::path() const {
        return path_;
    }

    std::ostream &operator<<(std::ostream &os, const Car &obj) {
        os << "[" << obj.id_ << " path_length: " << obj.path_.size() << "]";
        return os;
    }

}// namespace city_plan
