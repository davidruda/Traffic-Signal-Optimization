#ifndef CITY_PLAN_INTERSECTION_HPP
#define CITY_PLAN_INTERSECTION_HPP

#include <iostream>
#include <utility>
#include <vector>

namespace city_plan {
class Intersection {
public:
    explicit Intersection(size_t id)
        : id_(id) {}

    void set_used(bool used) {
        used_ = used;
    }

    bool used() const {
        return used_;
    }

    void set_non_trivial(bool non_trivial) {
        non_trivial_ = non_trivial;
    }

    bool non_trivial() const {
        return non_trivial_;
    }

    size_t id() const {
        return id_;
    }

    const std::vector<size_t> &incoming_streets() const {
        return incoming_streets_;
    }

    void set_incoming_streets(std::vector<size_t> &&incoming_streets) {
        incoming_streets_ = std::move(incoming_streets);
    }

    void add_incoming_street(size_t street_id) {
        incoming_streets_.emplace_back(street_id);
    }

    friend std::ostream &operator<<(std::ostream &os, const Intersection &obj);

private:
    const size_t id_;
    // incoming streets represented by street ids
    std::vector<size_t> incoming_streets_;
    bool used_{};
    bool non_trivial_{};
};
}

#endif
