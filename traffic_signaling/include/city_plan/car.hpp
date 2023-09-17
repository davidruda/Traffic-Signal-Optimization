#ifndef CITY_PLAN_CAR_HPP
#define CITY_PLAN_CAR_HPP

#include <iostream>
#include <vector>

namespace city_plan {
class Car {
public:
    Car(size_t id, std::vector<size_t> &&path)
        : id_(id), path_(std::move(path)) {}

    size_t id() const {
        return id_;
    }

    const std::vector<size_t> &path() const {
        return path_;
    }

    friend std::ostream &operator<<(std::ostream &os, const Car &obj) {
        os << "[" << obj.id_ << " path_length: " << obj.path_.size() << "]";
        return os;
    }

private:
    const size_t id_;
    // path represented with street ids
    const std::vector<size_t> path_;
};
}

#endif
