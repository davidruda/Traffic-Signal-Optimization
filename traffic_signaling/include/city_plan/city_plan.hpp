#ifndef CITY_PLAN_CITY_PLAN_HPP
#define CITY_PLAN_CITY_PLAN_HPP

#include <fstream>
#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>

#include "car.hpp"
#include "intersection.hpp"
#include "street.hpp"

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

    size_t duration() const {
        return duration_;
    }

    size_t bonus() const {
        return bonus_;
    }

    size_t street_id(std::string_view name) const {
        return street_mapping_.at(name);
    }

    friend std::ostream &operator<<(std::ostream &os, const CityPlan &obj);

private:
    void read_streets(std::ifstream &file, size_t count);
    void read_cars(std::ifstream &file, size_t count);

    size_t duration_;
    std::vector<Intersection> intersections_;
    std::vector<Street> streets_;
    std::vector<Car> cars_;
    size_t bonus_;
    // mapping from street name to street id
    std::unordered_map<std::string_view, size_t> street_mapping_;
};
}

#endif
