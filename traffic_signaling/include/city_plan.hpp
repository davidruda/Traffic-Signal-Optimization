#ifndef CITY_PLAN_HPP
#define CITY_PLAN_HPP

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "city_plan_car.hpp"
#include "city_plan_intersection.hpp"
#include "city_plan_street.hpp"

namespace city_plan {

    class CityPlan {
    public:
        explicit CityPlan(const std::string &filename);

        const std::vector<Intersection> &intersections() const;
        const std::vector<Street> &streets() const;
        const std::vector<Car> &cars() const;

        size_t duration() const;
        size_t bonus() const;

        size_t get_street_id_by_name(const std::string &name) const;

        friend std::ostream &operator<<(std::ostream &os, const CityPlan &obj);

    private:
        size_t duration_;
        std::vector<Intersection> intersections_;
        std::vector<Street> streets_;
        std::vector<Car> cars_;
        std::unordered_map<std::string_view, size_t> street_mapping_;
        size_t bonus_;
    };

}// namespace city_plan

#endif
