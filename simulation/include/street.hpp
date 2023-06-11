#ifndef STREET_HPP
#define STREET_HPP

#include <functional>
#include <queue>
#include <string>
#include <utility>

class Intersection;

#include "car.hpp"
//#include "intersection_.hpp"

class Street {
public:
    Street(
            size_t id,
            Intersection &start,
            Intersection &end,
            std::string name,
            size_t length);

    size_t id() const;
    const std::string &name() const;
    size_t length() const;
    bool is_used() const;
    void set_used(bool used);

    friend std::ostream &operator<<(std::ostream &os, const Street &obj);

    class Instance;

private:
    const size_t id_;
    Intersection &start_;
    Intersection &end_;
    const std::string name_;
    const size_t length_;
    bool used_;
};

class Street::Instance {
public:
    explicit Instance(const Street &data);

    size_t car_queue_size() const;
    void add_car(Car::Instance &car);
    Car::Instance &get_car(size_t time);

    size_t id() const;
    //Intersection &start() const;
    Intersection &end() const;
    const std::string &name() const;
    size_t length() const;

    int last_used_time() const;

private:
    const Street &data_;
    std::queue<std::reference_wrapper<Car::Instance>> car_queue_;
    int last_used_time_;
};

#endif
