
#include "car_instance.hpp"

CarInstance::CarInstance(const CarShared &data)
    : data_(data), street_in_path_index_(0), finished_(false), finish_time_(-1) {}


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
