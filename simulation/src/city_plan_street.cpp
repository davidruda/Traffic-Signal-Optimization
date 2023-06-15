#include <iostream>

#include "city_plan_street.hpp"

CityPlanStreet::CityPlanStreet(size_t id, size_t start_id, size_t end_id, std::string name, size_t length)
    : id_(id), start_id_(start_id), end_id_(end_id), name_(std::move(name)), length_(length), used_(false) {}

size_t CityPlanStreet::id() const {
    return id_;
}

size_t CityPlanStreet::end() const {
    return end_id_;
}

const std::string &CityPlanStreet::name() const {
    return name_;
}

void CityPlanStreet::set_used(bool used) {
    used_ = used;
}

bool CityPlanStreet::is_used() const {
    return used_;
}

size_t CityPlanStreet::length() const {
    return length_;
}

std::ostream &operator<<(std::ostream &os, const CityPlanStreet &obj) {
    os << "[" << obj.id_ << " " << obj.name_ << " " << obj.start_id_
       << " -> " << obj.end_id_ << " length: " << obj.length_ << "]";
    return os;
}
