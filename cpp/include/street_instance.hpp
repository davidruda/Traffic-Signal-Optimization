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

    void add_car(CarInstance &car);
    size_t car_queue_size() const;
    CarInstance &get_car(int time);

private:
    const StreetShared &data_;
    std::queue<std::reference_wrapper<CarInstance>> car_queue_;
    int last_used_time_;
};

#endif
