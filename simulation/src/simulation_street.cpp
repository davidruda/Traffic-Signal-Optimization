#include "simulation_street.hpp"

SimulationStreet::SimulationStreet(const CityPlanStreet &data) : data_(data) {}

void SimulationStreet::add_car(size_t car_id) {
    car_queue_.emplace(car_id);
}

size_t SimulationStreet::car_queue_size() const {
    return car_queue_.size();
}

size_t SimulationStreet::get_car(size_t time) {
    auto &&car = car_queue_.front();
    car_queue_.pop();
    last_used_time_ = time;
    return car;
}

size_t SimulationStreet::id() const {
    return data_.id();
}

size_t SimulationStreet::end() const {
    return data_.end();
}

const std::string &SimulationStreet::name() const {
    return data_.name();
}

size_t SimulationStreet::length() const {
    return data_.length();
}

std::optional<size_t> SimulationStreet::last_used_time() const {
    return last_used_time_;
}

bool SimulationStreet::is_used() const {
    return data_.is_used();
}

void SimulationStreet::reset() {
    car_queue_ = {};
    last_used_time_ = std::nullopt;
}