#include <string>
#include <fstream>
#include <iostream>

#include "simulation_shared.hpp"

//SimulationShared::SimulationShared(const std::string &filename) {
//    std::ifstream data_file{filename};
//    int duration;
//    int number_of_intersections;
//    int number_of_streets;
//    int number_of_cars;
//    int bonus;
//    data_file >> duration >> number_of_intersections >> number_of_streets
//            >> number_of_cars >> bonus;
//}

SimulationShared SimulationShared::from_file(const std::string &filename) {
    std::ifstream data_file{filename};
    int duration;
    int number_of_intersections;
    int number_of_streets;
    int number_of_cars;
    int bonus;
    data_file >> duration >> number_of_intersections >> number_of_streets
            >> number_of_cars >> bonus;

    std::cout << duration << " " << number_of_intersections << " "
            << number_of_streets << " " << number_of_cars << " "
            << bonus << "\n";

    std::vector<IntersectionShared> intersections;
    intersections.reserve(number_of_intersections);
    for (int i = 0; i < number_of_intersections; ++i) {
        intersections.emplace_back(i);
    }

    std::vector<StreetShared> streets;
    streets.reserve(number_of_streets);
    auto hash = [](const std::string* s) {
        return std::hash<std::string>{}(*s);
    };
    auto cmp = [](const std::string* l, const std::string* r){
        return *l == *r;
    };
    //std::unordered_map<const std::string*, StreetShared*, decltype(hash), decltype(cmp)> street_mapping;
    std::unordered_map<const std::string*, StreetShared*, StringPointerHash, StringPointerEqual> street_mapping;

    int start;
    int end;
    std::string name;
    int length;
    for (int i = 0; i < number_of_streets; ++i) {
        data_file >> start >> end >> name >> length;
        std::cout << start << " " << end << " " << name << " " << length << "\n";
        auto&& street = streets.emplace_back(i, &intersections[start], &intersections[end], name, length);
        street_mapping.emplace(&street.name(), &street);
        street.start()->outgoing().emplace_back(&street);
        street.end()->incoming().emplace_back(&street);
    }

    std::vector<CarShared> cars;
    cars.reserve(number_of_cars);

    int path_length;
    std::string street_name;

    for (int i = 0; i < number_of_cars; ++i) {
        data_file >> path_length;
        std::cout << path_length << " ";
        std::vector<const StreetShared*> path;
        for (int j = 0; j < path_length; ++j) {
            data_file >> street_name;
            std::cout << street_name << " ";
            auto&& street = street_mapping[&street_name];
            path.emplace_back(street);

            // The last street in path is not marked as used because it doesn't
            // use the traffic light
            if (j < path_length - 1) {
                street->set_used(true);
            }
        }
        cars.emplace_back(i, path_length, path);
        std::cout << "\n";
    }

    return {duration, intersections, streets, cars, street_mapping, bonus};
}
