#ifndef CITY_PLAN_INTERSECTION_HPP
#define CITY_PLAN_INTERSECTION_HPP

#include <iostream>
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

    const std::vector<size_t> &streets() const {
        return streets_;
    }

    void add_street(size_t street_id) {
        streets_.emplace_back(street_id);
    }

    friend std::ostream &operator<<(std::ostream &os, const Intersection &obj) {
        os << obj.id_ << "\n" << "Incoming streets:\n";
        for (auto &&s: obj.streets_) {
            os << s << "\n";
        }
        return os;
    }

private:
    const size_t id_;
    // incoming streets represented by street ids
    std::vector<size_t> streets_;
    bool used_{};
    bool non_trivial_{};
};
}

#endif
