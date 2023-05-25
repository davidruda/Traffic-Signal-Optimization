#include <fstream>
#include <iostream>
#include <string>

#include "simulation_shared.hpp"

SimulationShared::SimulationShared(const std::string &filename) {
    std::ifstream data_file{filename};
    int duration;
    int number_of_intersections;
    int number_of_streets;
    int number_of_cars;
    int bonus;

    data_file >> duration >> number_of_intersections >> number_of_streets >> number_of_cars >> bonus;

    duration_ = duration;
    bonus_ = bonus;

    std::cout << duration_ << " " << number_of_intersections << " "
              << number_of_streets << " " << number_of_cars << " "
              << bonus_ << "\n";

    intersections_.reserve(number_of_intersections);
    for (int i = 0; i < number_of_intersections; ++i) {
        intersections_.emplace_back(i);
    }

    streets_.reserve(number_of_streets);
    cars_.reserve(number_of_cars);


    int start;
    int end;
    std::string name;
    int length;
    for (int i = 0; i < number_of_streets; ++i) {
        data_file >> start >> end >> name >> length;
        std::cout << start << " " << end << " " << name << " " << length << "\n";
        auto &&street = streets_.emplace_back(i, intersections_[start], intersections_[end], name, length);
        street_mapping_.emplace(street.name(), std::ref(street));
        street.start().add_outgoing(street);
        street.end().add_incoming(street);
    }


    int path_length;
    std::string street_name;

    for (int i = 0; i < number_of_cars; ++i) {
        data_file >> path_length;
        std::cout << path_length << " ";
        std::vector<std::reference_wrapper<const StreetShared>> path;
        for (int j = 0; j < path_length; ++j) {
            data_file >> street_name;
            std::cout << street_name << " ";
            auto &&street = street_mapping_.at(street_name).get();
            path.emplace_back(std::ref(street));

            // The last street in path is not marked as used because it doesn't
            // use the traffic light
            if (j < path_length - 1) {
                street.set_used(true);
            }
        }
        cars_.emplace_back(i, path_length, path);
        std::cout << "\n";
    }
}

const std::vector<IntersectionShared> &SimulationShared::intersections() const {
    return intersections_;
}

const std::vector<StreetShared> &SimulationShared::streets() const {
    return streets_;
}

const std::vector<CarShared> &SimulationShared::cars() const {
    return cars_;
}

std::ostream &operator<<(std::ostream &os, const SimulationShared &obj) {
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
