#ifndef STREET_HPP
#define STREET_HPP

#include <functional>
#include <queue>
#include <string>
#include <utility>

class Intersection;
class Car;

#include "car.hpp"
#include "intersection.hpp"


class Street {
public:
    Street(
            int id,
            Intersection &start,
            Intersection &end,
            std::string name,
            int length);

    int id() const;
    Intersection &start() const;
    Intersection &end() const;
    const std::string &name() const;
    int length() const;
    bool is_used() const;
    void set_used(bool used);

    friend std::ostream &operator<<(std::ostream &os, const Street &obj);

    class Instance;

private:
    const int id_;
    Intersection &start_;
    Intersection &end_;
    const std::string name_;
    const int length_;
    bool used_;
};

class Street::Instance {
public:
    explicit Instance(const Street &data);

    size_t car_queue_size() const;
    void add_car(Car::Instance &car);
    Car::Instance &get_car(int time);

private:
    const Street &data_;
    std::queue<std::reference_wrapper<Car::Instance>> car_queue_;
    int last_used_time_;
};

#endif
