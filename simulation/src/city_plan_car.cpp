#include <iostream>

#include "city_plan_car.hpp"

CityPlanCar::CityPlanCar(size_t id, std::vector<size_t> path) : id_(id), path_(std::move(path)) {}

size_t CityPlanCar::id() const {
    return id_;
}

const std::vector<size_t> &CityPlanCar::path() const {
    return path_;
}

std::ostream &operator<<(std::ostream &os, const CityPlanCar &obj) {
    os << "[" << obj.id_ << " path_length: " << obj.path_.size() << "]";
    return os;
}
