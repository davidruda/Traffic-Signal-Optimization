#ifndef CAR_INSTANCE_HPP
#define CAR_INSTANCE_HPP

#include "car_shared.hpp"
#include "street_instance.hpp"

class StreetInstance;

class CarInstance {
public:
    explicit CarInstance(const CarShared &data);

    size_t current_street_index() const;

private:
    const CarShared &data_;
    size_t current_street_index_;
};

#endif
