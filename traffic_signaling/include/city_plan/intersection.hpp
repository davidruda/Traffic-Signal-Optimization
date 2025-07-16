#ifndef CITY_PLAN_INTERSECTION_HPP
#define CITY_PLAN_INTERSECTION_HPP

#include <vector>
#include <unordered_map>
#include <functional>

// circular dependency resolved by forward declaration
#include "city_plan/street.hpp"

namespace city_plan {
/**
 * Intersection in the city plan.
 *
 * Contains all "static" information about an intersection known from the input data.
 */
class Intersection {
public:
    /**
     * Construct an intersection object.
     *
     * @param id ID of the intersection.
     */
    explicit Intersection(unsigned long id) : id_(id) {}

    /**
     * Return True if the intersection is used.
     *
     * Used intersection has at least one used street.
     */
    bool used() const {
        return !used_streets_.empty();
    }

    /**
     * Return True if the intersection is non-trivial.
     *
     * Non-trivial intersection has two or more used streets.
     */
    bool non_trivial() const {
        return used_streets_.size() > 1;
    }

    /**
     * Return the ID of the intersection.
     */
    unsigned long id() const {
        return id_;
    }

    /**
     * Return the incoming streets to the intersection.
     */
    const std::vector<std::reference_wrapper<const Street>> &streets() const {
        return streets_;
    }

    /**
     * Return the used incoming streets to the intersection.
     */
    const std::vector<std::reference_wrapper<const Street>> &used_streets() const {
        return used_streets_;
    }

    /**
     * Add a street to the internal `streets_` vector.
     *
     * @param street Street to add.
     *
     * This method is only used during CityPlan initialization.
     */
    void add_street(const Street& street) {
        streets_.emplace_back(street);
    }

    /**
     * Add a used street to the internal `used_streets_` vector.
     *
     * @param street Street to add.
     *
     * This method is only used during CityPlan initialization.
     */
    void add_used_street(const Street& street) {
        street_index_[street.id()] = static_cast<unsigned long>(used_streets_.size());
        used_streets_.emplace_back(street);
    }

    /**
     * Return the index relative to this intersection for the given street.
     *
     * @param street_id ID of the street.
     *
     * This method is only valid for used streets.
     */
    unsigned long street_index(unsigned long street_id) const {
        return street_index_.at(street_id);
    }

private:
    /** ID of the intersection. */
    unsigned long id_;
    /** Incoming streets to this intersection. */
    std::vector<std::reference_wrapper<const Street>> streets_;
    /** Used incoming streets to this intersection. */
    std::vector<std::reference_wrapper<const Street>> used_streets_;

    /**
     * A mapping from street ID to an index relative to this intersection.
     *
     * Only valid for used streets.
     */
    std::unordered_map<unsigned long, unsigned long> street_index_;
};
}

#endif
