#ifndef CITY_PLAN_STREET_HPP
#define CITY_PLAN_STREET_HPP

#include <string>

namespace city_plan {
// necessary forward declaration to resolve circular dependency
class Intersection;

/**
 * Street in the city plan.
 *
 * Contains all "static" information about a street known from the input data.
 */
class Street {
public:
    /**
     * Construct a street object.
     *
     * @param id ID of the street.
     * @param start Starting intersection of the street.
     * @param end Ending intersection of the street.
     * @param name Name of the street.
     * @param length Length of the street in seconds it takes to drive through it.
     */
    Street(unsigned long id, const Intersection &start, const Intersection &end,
           std::string &&name, unsigned long length)
        : id_(id), start_(start), end_(end),
          name_(std::move(name)), length_(length) {}

    /**
     * Return the ID of the street.
     */
    unsigned long id() const {
        return id_;
    }

    /**
     * Return the starting intersection of the street.
     */
    const Intersection &start() const {
        return start_;
    }

    /**
     * Return the ending intersection of the street.
     */
    const Intersection &end() const {
        return end_;
    }

    /**
     * Return the name of the street.
     */
    const std::string &name() const {
        return name_;
    }

    /**
     * Increase the number of cars that use this street by one.
     *
     * This method is only used during CityPlan initialization.
     */
    void add_car() {
        ++total_cars_;
    }

    /**
     * Return the total number of cars that use this street.
     */
    unsigned long total_cars() const {
        return total_cars_;
    }

    /**
     * Return True if the street is used.
     *
     * A street is used if it is a non-final street in the path of at least one car.
     */
    bool used() const {
        return total_cars_ > 0;
    }

    /**
     * Return the duration in seconds to drive through the street.
     */
    unsigned long length() const {
        return length_;
    }

private:
    /** ID of the street. */
    unsigned long id_;
    /** Starting intersection of the street. */
    const Intersection &start_;
    /** Ending intersection of the street. */
    const Intersection &end_;
    /** Name of the street. */
    std::string name_;
    /** Length of the street in seconds it takes to drive through it. */
    unsigned long length_;
    /** Total number of cars that use this street. */
    unsigned long total_cars_{};
};
}

#endif
