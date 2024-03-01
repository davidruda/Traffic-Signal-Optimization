#include <stdexcept>
#include <set>
#include <functional>

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
    unsigned long number_of_intersections;
    unsigned long number_of_streets;
    unsigned long number_of_cars;

    file >> duration_ >> number_of_intersections >> number_of_streets
         >> number_of_cars >> bonus_;

    intersections_.reserve(number_of_intersections);
    streets_.reserve(number_of_streets);
    street_mapping_.reserve(number_of_streets);
    cars_.reserve(number_of_cars);

    for (unsigned long id = 0; id < number_of_intersections; ++id) {
        intersections_.emplace_back(id);
    }
    read_streets(file, number_of_streets);
    read_cars(file, number_of_cars);
}

void CityPlan::read_streets(std::ifstream &file, unsigned long count) {
    unsigned long start_id;
    unsigned long end_id;
    std::string name;
    unsigned long length;
    for (unsigned long id = 0; id < count; ++id) {
        file >> start_id >> end_id >> name >> length;
        auto &&start = intersections_[start_id];
        auto &&end = intersections_[end_id];
        auto &&street = streets_.emplace_back(id, start, end, std::move(name), length);
        street_mapping_.emplace(street.name(), id);
        intersections_[end_id].add_street(street);
    }
}

void CityPlan::read_cars(std::ifstream &file, unsigned long count) {
    unsigned long path_length;
    std::string street_name;

    std::unordered_map<unsigned long, std::set<unsigned long>> used_streets;
    for (unsigned long id = 0; id < count; ++id) {
        file >> path_length;

        std::vector<std::reference_wrapper<const Street>> path;
        path.reserve(path_length);
        for (unsigned long i = 0; i < path_length; ++i) {
            file >> street_name;
            auto &&street_id = street_mapping_[street_name];
            path.emplace_back(streets_[street_id]);

            // The last street in path is not used because the car
            // doesn't use the traffic light there
            if (i < path_length - 1) {
                streets_[street_id].add_car();
                used_streets[streets_[street_id].end().id()].emplace(street_id);
            }
        }
        cars_.emplace_back(id, std::move(path));
    }

    // Make sure to add used streets in the increasing order of intersection ids
    for (auto &&[intersection_id, street_ids]: used_streets) {
        for (auto &&street_id: street_ids) {
            intersections_[intersection_id].add_used_street(streets_[street_id]);
        }
    }
}
}
