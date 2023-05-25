#ifndef STREET_INSTANCE_HPP
#define STREET_INSTANCE_HPP

#include <functional>
#include <queue>

#include "car_instance.hpp"
#include "street_shared.hpp"

class CarInstance;

class StreetInstance {
public:
    explicit StreetInstance(const StreetShared &data);

    void add_car(const CarInstance &car);

private:
    const StreetShared &data_;
    std::queue<std::reference_wrapper<const CarInstance>> queueing_cars_;
};

#endif
