
#include <iostream>

#include "car.hpp"

Car::Car(
        int id,
        int path_length,
        std::vector<std::reference_wrapper<const Street>> path) : id_(id), path_length_(path_length), path_(std::move(path)) {}

int Car::id() const {
    return id_;
}

int Car::path_length() const {
    return path_length_;
}

const std::vector<std::reference_wrapper<const Street>> &Car::path() const {
    return path_;
}

std::ostream &operator<<(std::ostream &os, const Car &obj) {
    os << "[" << obj.id_ << " path_length: " << obj.path_length_ << "]";
    return os;
}

Car::Instance::Instance(const Car &data)
    : data_(data), street_in_path_index_(0), finish_time_(-1), finished_(false) {}


// TODO: current_street is the current Street, not Instance, it's not clear
const Street &Car::Instance::current_street() const {
    return data_.path()[street_in_path_index_];
}

void Car::Instance::move_to_next_street() {
    ++street_in_path_index_;
}

bool Car::Instance::at_final_destination() const {
    return data_.path().size() - 1 == street_in_path_index_;
}

void Car::Instance::set_finished(bool finished) {
    finished_ = finished;
}

void Car::Instance::set_finish_time(int finishTime) {
    finish_time_ = finishTime;
}

int Car::Instance::finish_time() const {
    return finish_time_;
}

bool Car::Instance::finished() const {
    return finished_;
}

int Car::Instance::id() const {
    return data_.id_;
}

int Car::Instance::path_length() const {
    return data_.path_length_;
}

const std::vector<std::reference_wrapper<const Street>> &Car::Instance::path() const {
    return data_.path_;
}
