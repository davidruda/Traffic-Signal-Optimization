#include <iostream>
#include <stdexcept>
#include <utility>
#include <set>

#include "city_plan/city_plan.hpp"

namespace city_plan {

CityPlan::CityPlan(const std::string &filename) { // NOLINT(*-pro-type-member-init)
    std::ifstream file{filename};
    if (!file.is_open()) {
        throw std::runtime_error{"Could not open file " + filename};
    }
    *this = CityPlan{file};
}

CityPlan::CityPlan(std::ifstream &file) { // NOLINT(*-pro-type-member-init)
    size_t number_of_intersections;
    size_t number_of_streets;
    size_t number_of_cars;

    file >> duration_ >> number_of_intersections >> number_of_streets
         >> number_of_cars >> bonus_;

    intersections_.reserve(number_of_intersections);
    streets_.reserve(number_of_streets);
    street_mapping_.reserve(number_of_streets);
    cars_.reserve(number_of_cars);

    for (size_t id = 0; id < number_of_intersections; ++id) {
        intersections_.emplace_back(id);
    }
    read_streets(file, number_of_streets);
    read_cars(file, number_of_cars);
}

void CityPlan::read_streets(std::ifstream &file, size_t count) {
    size_t start;
    size_t end;
    std::string name;
    size_t length;
    for (size_t id = 0; id < count; ++id) {
        file >> start >> end >> name >> length;
        auto &&street = streets_.emplace_back(id, start, end, std::move(name), length);
        street_mapping_.emplace(street.name(), id);
        intersections_[end].add_street(id);
    }
}

void CityPlan::read_cars(std::ifstream &file, size_t count) {
    size_t path_length;
    std::string street_name;

    std::unordered_map<size_t, std::set<size_t>> used_streets;
    for (size_t id = 0; id < count; ++id) {
        file >> path_length;

        std::vector<size_t> path;
        path.reserve(path_length);
        for (size_t i = 0; i < path_length; ++i) {
            file >> street_name;
            auto &&street_id = street_mapping_[street_name];
            path.emplace_back(street_id);

            // The last street in path is not used because the car
            // doesn't use the traffic light there
            if (i < path_length - 1) {
                streets_[street_id].add_car();
                //intersections_[streets_[street_id].end()].add_used_street(street_id);
                used_streets[streets_[street_id].end()].emplace(street_id);
            }
        }
        cars_.emplace_back(id, std::move(path));
    }

    // Make sure to add used streets in the increasing order of intersection ids
    for (auto &&[intersection_id, street_ids]: used_streets) {
        for (auto &&street_id: street_ids) {
            intersections_[intersection_id].add_used_street(street_id);
        }
    }
}

std::ostream &operator<<(std::ostream &os, const CityPlan &obj) {
    os << "Duration: " << obj.duration_ << "\n"
       << "Number of intersections: " << obj.intersections_.size() << "\n"
       << "Bonus: " << obj.bonus_ << "\n"
       << "Number of streets: " << obj.streets_.size() << "\n";
    for (auto &&s: obj.streets_) {
        os << s << "\n";
    }
    os << "Number of cars: " << obj.cars_.size() << "\n";
    for (auto &&c: obj.cars_) {
        os << c << "\n";
    }
    return os;
}
}
