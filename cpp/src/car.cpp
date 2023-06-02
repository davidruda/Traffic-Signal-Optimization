
#include <iostream>

#include "car.hpp"

CarShared::CarShared(
        int id,
        int path_length,
        std::vector<std::reference_wrapper<const StreetShared>> path) : id_(id), path_length_(path_length), path_(std::move(path)) {}

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

CarInstance::CarInstance(const CarShared &data)
    : data_(data), street_in_path_index_(0), finish_time_(-1), finished_(false) {}


// TODO: current_street is the current StreetShared, not StreetInstance, it's not clear
const StreetShared &CarInstance::current_street() const {
    return data_.path()[street_in_path_index_];
}

void CarInstance::move_to_next_street() {
    ++street_in_path_index_;
}

bool CarInstance::at_final_destination() const {
    return data_.path().size() - 1 == street_in_path_index_;
}

const CarShared &CarInstance::data() const {
    return data_;
}

void CarInstance::set_finished(bool finished) {
    finished_ = finished;
}

void CarInstance::set_finish_time(int finishTime) {
    finish_time_ = finishTime;
}

int CarInstance::finish_time() const {
    return finish_time_;
}

bool CarInstance::finished() const {
    return finished_;
}