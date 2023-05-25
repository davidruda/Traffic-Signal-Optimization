
#include <functional>

#include "street_instance.hpp"

StreetInstance::StreetInstance(const StreetShared &data) : data_(data) {}

void StreetInstance::add_car(const CarInstance &car) {
    queueing_cars_.emplace(std::cref(car));
}
