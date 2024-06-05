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
class CityPlan {
public:
    explicit CityPlan(const std::string &filename);
    explicit CityPlan(std::ifstream &file);

    const std::vector<Intersection> &intersections() const {
        return intersections_;
    }

    const std::vector<Street> &streets() const {
        return streets_;
    }

    const std::vector<Car> &cars() const {
        return cars_;
    }

    unsigned long duration() const {
        return duration_;
    }

    unsigned long bonus() const {
        return bonus_;
    }

    auto used_intersections() const {
        return intersections_ | std::ranges::views::filter(&Intersection::used);
    }

    auto non_trivial_intersections() const {
        return intersections_ | std::ranges::views::filter(&Intersection::non_trivial);
    }

    unsigned long street_id(std::string_view name) const {
        return street_mapping_.at(name);
    }

    // Theoretical maximum score if none of the cars ever has to wait at a traffic light
    unsigned long upper_bound() const;

private:
    void read_streets(std::ifstream &file, unsigned long count);
    void read_cars(std::ifstream &file, unsigned long count);

    unsigned long duration_;
    std::vector<Intersection> intersections_;
    std::vector<Street> streets_;
    std::vector<Car> cars_;
    unsigned long bonus_;
    // mapping from street name to street id
    std::unordered_map<std::string_view, unsigned long> street_mapping_;
};
}

#endif
