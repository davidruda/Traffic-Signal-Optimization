#ifndef STREET_INSTANCE_HPP
#define STREET_INSTANCE_HPP

#include <queue>
#include <functional>

#include "street_shared.hpp"
#include "car_instance.hpp"

class CarInstance;

class StreetInstance {
public:
    explicit StreetInstance(const StreetShared &data);

    void add_car(const CarInstance& car);

private:
    const StreetShared & data_;
    std::queue<std::reference_wrapper<const CarInstance>> queueing_cars_;
};

#endif
