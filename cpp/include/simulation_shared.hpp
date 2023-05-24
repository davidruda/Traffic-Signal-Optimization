#ifndef SIMULATION_SHARED_HPP
#define SIMULATION_SHARED_HPP

#include <utility>
#include <vector>
#include <unordered_map>
#include <string>

#include "intersection_shared.hpp"
#include "street_shared.hpp"
#include "car_shared.hpp"

struct StringPointerHash;
struct StringPointerEqual;

class SimulationShared {
public:
    //explicit SimulationShared(const std::string& filename);
    static SimulationShared from_file(const std::string& filename);

    SimulationShared(
        int duration,
        std::vector<IntersectionShared> intersections,
        std::vector<StreetShared> streets,
        std::vector<CarShared> cars,
        std::unordered_map<const std::string *, StreetShared *, StringPointerHash, StringPointerEqual> street_mapping,
        int bonus
    ) : duration_(duration), intersections_(std::move(intersections)), streets_(std::move(streets)), cars_(std::move(cars)), street_mapping_(std::move(street_mapping)), bonus_(bonus) {}

private:
    const int duration_;
    const std::vector<IntersectionShared> intersections_;
    const std::vector<StreetShared> streets_;
    const std::vector<CarShared> cars_;
    const std::unordered_map<const std::string *, StreetShared*, StringPointerHash, StringPointerEqual> street_mapping_;
    const int bonus_;
};

struct StringPointerHash {
    size_t operator()(const std::string* s) {
        return std::hash<std::string>{}(*s);
    }
};

struct StringPointerEqual {
    bool operator()(const std::string* lhs, const std::string* rhs) {
        return *lhs == *rhs;
    }
};

#endif
