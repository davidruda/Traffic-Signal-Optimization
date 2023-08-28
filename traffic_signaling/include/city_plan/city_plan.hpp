#ifndef CITY_PLAN_CITY_PLAN_HPP
#define CITY_PLAN_CITY_PLAN_HPP

#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "car.hpp"
#include "intersection.hpp"
#include "street.hpp"

namespace city_plan {

    class CityPlan {
    public:
        explicit CityPlan(const std::string &filename);

        CityPlan(std::vector<Car> cars,
                 std::vector<Intersection> intersections,
                 std::vector<Street> streets,
                 size_t bonus,
                 size_t duration) : duration_(duration), intersections_(std::move(intersections)), streets_(std::move(streets)), cars_(std::move(cars)), bonus_(bonus) {}

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

        size_t get_street_id_by_name(const std::string &name) const {
            return street_mapping_.at(name);
        }

        friend std::ostream &operator<<(std::ostream &os, const CityPlan &obj);

    private:
        size_t duration_;
        std::vector<Intersection> intersections_;
        std::vector<Street> streets_;
        std::vector<Car> cars_;
        std::unordered_map<std::string_view, size_t> street_mapping_;
        size_t bonus_;
    };

}

#endif
