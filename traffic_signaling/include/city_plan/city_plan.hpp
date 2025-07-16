#ifndef CITY_PLAN_CITY_PLAN_HPP
#define CITY_PLAN_CITY_PLAN_HPP

#include <fstream>
#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <ranges>

#include "city_plan/intersection.hpp"
#include "city_plan/street.hpp"
#include "city_plan/car.hpp"

namespace city_plan {
/**
 * City plan of a given dataset.
 *
 * Contains all "static" information about intersections, streets, and cars known from the input data.
 */
class CityPlan {
public:
    /**
     * Construct a city plan from the input data file.
     *
     * @param filename Path of the file containing the input data.
     */
    explicit CityPlan(const std::string &filename);

    /**
     * Return the intersections in the city plan.
     */
    const std::vector<Intersection> &intersections() const {
        return intersections_;
    }

    /**
     * Return the streets in the city plan.
     */
    const std::vector<Street> &streets() const {
        return streets_;
    }

    /**
     * Return the cars in the city plan.
     */
    const std::vector<Car> &cars() const {
        return cars_;
    }

    /**
     * Return the duration of the simulation for this city plan.
     */
    unsigned long duration() const {
        return duration_;
    }

    /**
     * Return the bonus awarded for each car that reaches its destination before the end of the simulation.
     */
    unsigned long bonus() const {
        return bonus_;
    }

    /**
     * Return the used intersections in this city plan.
     */
    auto used_intersections() const {
        return intersections_ | std::views::filter(&Intersection::used);
    }

    /**
     * Return the non-trivial intersections in this city plan.
     */
    auto non_trivial_intersections() const {
        return intersections_ | std::views::filter(&Intersection::non_trivial);
    }

    /**
     * Return the ID of a street given its name.
     *
     * @param name Name of the street.
     */
    unsigned long street_id(std::string_view name) const {
        return street_mapping_.at(name);
    }

    /**
     * Return the theoretical maximum score if none of the cars ever has to wait at a traffic light.
     */
    unsigned long upper_bound() const;

private:
    /**
     * Construct a city plan from an input stream.
     *
     * @param file Input file stream to read from.
     */
    explicit CityPlan(std::ifstream &file);

    /**
     * Read streets from the input file.
     *
     * @param file Input file stream to read from.
     * @param count Number of streets to read.
     */
    void read_streets(std::ifstream &file, unsigned long count);

    /**
     * Read cars from the input file.
     *
     * @param file Input file stream to read from.
     * @param count Number of cars to read.
     */
    void read_cars(std::ifstream &file, unsigned long count);

    /** Duration of the simulation in seconds. */
    unsigned long duration_;
    /** Interactions in this city plan. */
    std::vector<Intersection> intersections_;
    /** Streets in this city plan. */
    std::vector<Street> streets_;
    /** Cars in this city plan. */
    std::vector<Car> cars_;
    /** Bonus awarded for each car that reaches its destination before the end of the simulation. */
    unsigned long bonus_;
    /** Mapping from street name to street ID. */
    std::unordered_map<std::string_view, unsigned long> street_mapping_;
};
}

#endif
