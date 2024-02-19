#ifndef CITY_PLAN_INTERSECTION_HPP
#define CITY_PLAN_INTERSECTION_HPP

#include <iostream>
#include <vector>
#include <unordered_map>

namespace city_plan {
class Intersection {
public:
    explicit Intersection(size_t id)
        : id_(id) {}

    bool used() const {
        return !used_streets_.empty();
    }

    bool non_trivial() const {
        return used_streets_.size() > 1;
    }

    size_t id() const {
        return id_;
    }

    const std::vector<size_t> &streets() const {
        return streets_;
    }

    const std::vector<size_t> &used_streets() const {
        return used_streets_;
    }

    void add_street(size_t street_id) {
        streets_.emplace_back(street_id);
    }

    void add_used_street(size_t street_id) {
        street_index_[street_id] = used_streets_.size();
        used_streets_.push_back(street_id);
    }

    // returns the index of the street_id relative to this intersection
    // only valid for used streets
    size_t street_index(size_t street_id) const {
        return street_index_.at(street_id);
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
    std::vector<size_t> used_streets_;
    // a mapping from used street_id to an index relative to this intersection
    std::unordered_map<size_t, size_t> street_index_;
};
}

#endif
