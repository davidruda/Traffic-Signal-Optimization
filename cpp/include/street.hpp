#ifndef STREET_HPP
#define STREET_HPP

#include <functional>
#include <queue>
#include <string>
#include <utility>

#include "car.hpp"
#include "intersection.hpp"

class IntersectionShared;

class StreetShared {
public:
    StreetShared(
            int id,
            IntersectionShared &start,
            IntersectionShared &end,
            std::string name,
            int length);

    int id() const;
    IntersectionShared &start() const;
    IntersectionShared &end() const;
    const std::string &name() const;
    int length() const;
    bool is_used() const;
    void set_used(bool used);

    friend std::ostream &operator<<(std::ostream &os, const StreetShared &obj);

private:
    const int id_;
    IntersectionShared &start_;
    IntersectionShared &end_;
    const std::string name_;
    const int length_;
    bool used_;
};

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
