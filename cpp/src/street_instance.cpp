
#include <functional>

#include "street_instance.hpp"

StreetInstance::StreetInstance(const StreetShared &data) : data_(data), last_used_time_(-1) {}

void StreetInstance::add_car(CarInstance &car) {
    car_queue_.emplace(std::ref(car));
}

size_t StreetInstance::car_queue_size() const {
    return car_queue_.size();
}

CarInstance &StreetInstance::get_car(int time) {
    auto &&car = car_queue_.front().get();
    car_queue_.pop();
    last_used_time_ = time;
    return car;
}
