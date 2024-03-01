#ifndef CITY_PLAN_INTERSECTION_HPP
#define CITY_PLAN_INTERSECTION_HPP

#include <vector>
#include <unordered_map>
#include <functional>

// circular dependency resolved by forward declaration
#include "city_plan/street.hpp"

namespace city_plan {
class Intersection {
public:
    explicit Intersection(unsigned long id) : id_(id) {}

    bool used() const {
        return !used_streets_.empty();
    }

    bool non_trivial() const {
        return used_streets_.size() > 1;
    }

    unsigned long id() const {
        return id_;
    }

    const std::vector<std::reference_wrapper<const Street>> &streets() const {
        return streets_;
    }

    const std::vector<std::reference_wrapper<const Street>> &used_streets() const {
        return used_streets_;
    }

    void add_street(const Street& street) {
        streets_.emplace_back(street);
    }

    void add_used_street(const Street& street) {
        street_index_[street.id()] = static_cast<unsigned long>(used_streets_.size());
        used_streets_.emplace_back(street);
    }

    // returns the index of the street_id relative to this intersection
    // only valid for used streets
    unsigned long street_index(unsigned long street_id) const {
        return street_index_.at(street_id);
    }

private:
    unsigned long id_;
    // incoming streets represented by street ids
    std::vector<std::reference_wrapper<const Street>> streets_;
    std::vector<std::reference_wrapper<const Street>> used_streets_;
    // a mapping from used street_id to an index relative to this intersection
    std::unordered_map<unsigned long, unsigned long> street_index_;
};
}

#endif
