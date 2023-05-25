
#include "car_instance.hpp"

CarInstance::CarInstance(const CarShared &data)
    : data_(data), current_street_index_(0) {}

size_t CarInstance::current_street_index() const {
    return current_street_index_;
}
