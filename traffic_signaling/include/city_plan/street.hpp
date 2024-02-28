#ifndef CITY_PLAN_STREET_HPP
#define CITY_PLAN_STREET_HPP

#include <string>

namespace city_plan {
// necessary forward declaration to resolve circular dependency
class Intersection;

class Street {
public:
    Street(size_t id, const Intersection &start, const Intersection &end,
           std::string &&name, size_t length)
        : id_(id), start_(start), end_(end),
          name_(std::move(name)), length_(length) {}

    size_t id() const {
        return id_;
    }

    const Intersection &start() const {
        return start_;
    }

    const Intersection &end() const {
        return end_;
    }

    const std::string &name() const {
        return name_;
    }

    void add_car() {
        ++total_cars_;
    }

    size_t total_cars() const {
        return total_cars_;
    }

    bool used() const {
        return total_cars_ > 0;
    }

    size_t length() const {
        return length_;
    }

private:
    size_t id_;
    const Intersection &start_;
    const Intersection &end_;
    std::string name_;
    size_t length_;
    // number of cars that use this street
    size_t total_cars_{};
    // TODO: maybe use a vector of cars instead of a counter
};
}

#endif
