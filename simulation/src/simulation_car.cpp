#include <limits>

#include "simulation_car.hpp"

SimulationCar::SimulationCar(const CityPlanCar &data)
    : data_(data), path_index_(0), finish_time_(std::numeric_limits<size_t>::max()), finished_(false) {}

size_t SimulationCar::current_street() const {
    return data_.path()[path_index_];
}

void SimulationCar::move_to_next_street() {
    ++path_index_;
}

bool SimulationCar::at_final_destination() const {
    return data_.path().size() - 1 == path_index_;
}

void SimulationCar::set_finished(bool finished) {
    finished_ = finished;
}

void SimulationCar::set_finish_time(size_t finishTime) {
    finish_time_ = finishTime;
}

size_t SimulationCar::finish_time() const {
    return finish_time_;
}

bool SimulationCar::finished() const {
    return finished_;
}

size_t SimulationCar::id() const {
    return data_.id();
}

void SimulationCar::reset() {
    path_index_ = 0;
    finish_time_ = std::numeric_limits<size_t>::max();
    finished_ = false;
}