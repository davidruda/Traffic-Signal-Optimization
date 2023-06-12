
#include <iostream>
#include <limits>

#include "car.hpp"

Car::Car(
        size_t id,
        size_t path_length,
        std::vector<size_t> path) : id_(id), path_length_(path_length), path_(std::move(path)) {}

std::ostream &operator<<(std::ostream &os, const Car &obj) {
    os << "[" << obj.id_ << " path_length: " << obj.path_length_ << "]";
    return os;
}

Car::Instance::Instance(const Car &data)
    : data_(data), path_index_(0), finish_time_(std::numeric_limits<size_t>::max()), finished_(false) {}

size_t Car::Instance::current_street() const {
    return data_.path_[path_index_];
}

void Car::Instance::move_to_next_street() {
    ++path_index_;
}

bool Car::Instance::at_final_destination() const {
    return data_.path_.size() - 1 == path_index_;
}

void Car::Instance::set_finished(bool finished) {
    finished_ = finished;
}

void Car::Instance::set_finish_time(size_t finishTime) {
    finish_time_ = finishTime;
}

size_t Car::Instance::finish_time() const {
    return finish_time_;
}

bool Car::Instance::finished() const {
    return finished_;
}

size_t Car::Instance::id() const {
    return data_.id_;
}
