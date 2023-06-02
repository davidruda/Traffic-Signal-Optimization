
#include <functional>
#include <iostream>

#include "street.hpp"

Street::Street(
        int id,
        Intersection &start,
        Intersection &end,
        std::string name,
        int length) : id_(id), start_(start), end_(end), name_(std::move(name)), length_(length), used_(false) {}

int Street::id() const {
    return id_;
}

Intersection &Street::start() const {
    return start_;
}

Intersection &Street::end() const {
    return end_;
}

const std::string &Street::name() const {
    return name_;
}

int Street::length() const {
    return length_;
}

bool Street::is_used() const {
    return used_;
}

void Street::set_used(bool used) {
    used_ = used;
}

std::ostream &operator<<(std::ostream &os, const Street &obj) {
    os << "[" << obj.id_ << " " << obj.name_ << " " << obj.start_.id()
       << " -> " << obj.end_.id() << " length: " << obj.length_ << "]";
    return os;
}

Street::Instance::Instance(const Street &data) : data_(data), last_used_time_(-1) {}

void Street::Instance::add_car(Car::Instance &car) {
    car_queue_.emplace(std::ref(car));
}

size_t Street::Instance::car_queue_size() const {
    return car_queue_.size();
}

Car::Instance &Street::Instance::get_car(int time) {
    auto &&car = car_queue_.front().get();
    car_queue_.pop();
    last_used_time_ = time;
    return car;
}
