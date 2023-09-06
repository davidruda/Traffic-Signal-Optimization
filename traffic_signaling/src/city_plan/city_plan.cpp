#include <iostream>
#include <stdexcept>

#include "city_plan/city_plan.hpp"

namespace city_plan {

CityPlan::CityPlan(const std::string &filename) { // NOLINT(*-pro-type-member-init)
    std::ifstream file{filename};
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }
    *this = CityPlan{file};
}

CityPlan::CityPlan(std::ifstream &file) {
    size_t duration;
    size_t number_of_intersections;
    size_t number_of_streets;
    size_t number_of_cars;
    size_t bonus;
    std::unordered_map<std::string_view, size_t> street_mapping;

    file >> duration >> number_of_intersections >> number_of_streets >> number_of_cars >> bonus;

    duration_ = duration;
    bonus_ = bonus;

#ifdef DEBUG
    std::cout << duration_ << " " << number_of_intersections << " "
              << number_of_streets << " " << number_of_cars << " "
              << bonus_ << "\n";
#endif

    intersections_.reserve(number_of_intersections);
    for (size_t id = 0; id < number_of_intersections; ++id) {
        intersections_.emplace_back(id);
    }

    streets_.reserve(number_of_streets);
    street_mapping.reserve(number_of_streets);
    cars_.reserve(number_of_cars);

    size_t start;
    size_t end;
    std::string name;
    size_t length;
    for (size_t id = 0; id < number_of_streets; ++id) {
        file >> start >> end >> name >> length;
#ifdef DEBUG
        std::cout << start << " " << end << " " << name << " " << length << "\n";
#endif
        auto &&street = streets_.emplace_back(id, start, end, std::move(name), length);
        street_mapping.emplace(street.name(), id);
        intersections_[end].add_incoming_street(id);
    }

    size_t path_length;
    std::string street_name;

    for (size_t id = 0; id < number_of_cars; ++id) {
        file >> path_length;
#ifdef DEBUG
        std::cout << path_length << " ";
#endif
        std::vector<size_t> path;
        path.reserve(path_length);
        for (size_t i = 0; i < path_length; ++i) {
            file >> street_name;
#ifdef DEBUG
            std::cout << street_name << " ";
#endif
            auto &&street_id = street_mapping[street_name];
            path.emplace_back(street_id);

            // The last street in path is not marked as used because it doesn't
            // use the traffic light
            if (i < path_length - 1) {
                streets_[street_id].set_used(true);
                intersections_[streets_[street_id].end()].set_used(true);
            }
        }
        cars_.emplace_back(id, std::move(path));
#ifdef DEBUG
        std::cout << "\n";
#endif
    }

    for (auto &&intersection: intersections_) {
        if (intersection.used()) {
            size_t used_streets = 0;
            for (auto &&street_id: intersection.incoming_streets()) {
                if (streets_[street_id].used()) {
                    ++used_streets;
                    if (used_streets >= 2) {
                        intersection.set_non_trivial(true);
                        break;
                    }
                }
            }
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
