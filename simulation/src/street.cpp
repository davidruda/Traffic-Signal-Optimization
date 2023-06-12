
#include <functional>
#include <iostream>

#include "street.hpp"

Street::Street(
        size_t id,
        size_t start_id,
        size_t end_id,
        std::string name,
        size_t length) : id_(id), start_id_(start_id), end_id_(end_id), name_(std::move(name)), length_(length), used_(false) {}

const std::string &Street::name() const {
    return name_;
}

void Street::set_used(bool used) {
    used_ = used;
}

std::ostream &operator<<(std::ostream &os, const Street &obj) {
    os << "[" << obj.id_ << " " << obj.name_ << " " << obj.start_id_
       << " -> " << obj.end_id_ << " length: " << obj.length_ << "]";
    return os;
}

Street::Instance::Instance(const Street &data) : data_(data) {}

void Street::Instance::add_car(size_t car_id) {
    car_queue_.emplace(car_id);
}

size_t Street::Instance::car_queue_size() const {
    return car_queue_.size();
}

size_t Street::Instance::get_car(size_t time) {
    auto &&car = car_queue_.front();
    car_queue_.pop();
    last_used_time_ = time;
    return car;
}

size_t Street::Instance::id() const {
    return data_.id_;
}

size_t Street::Instance::end() const {
    return data_.end_id_;
}

const std::string &Street::Instance::name() const {
    return data_.name_;
}

size_t Street::Instance::length() const {
    return data_.length_;
}

std::optional<size_t> Street::Instance::last_used_time() const {
    return last_used_time_;
}

bool Street::Instance::is_used() const {
    return data_.used_;
}
