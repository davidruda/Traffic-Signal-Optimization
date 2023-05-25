#ifndef SIMULATION_SHARED_HPP
#define SIMULATION_SHARED_HPP

#include <utility>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>

#include "intersection_shared.hpp"
#include "street_shared.hpp"
#include "car_shared.hpp"


class SimulationShared {
public:
    explicit SimulationShared(const std::string& filename);
    //static SimulationShared from_file(const std::string& filename);

    friend std::ostream& operator<<(std::ostream& os, const SimulationShared& obj);

private:
    int duration_;
    std::vector<IntersectionShared> intersections_;
    std::vector<StreetShared> streets_;
    std::vector<CarShared> cars_;
    std::unordered_map<std::string_view, std::reference_wrapper<StreetShared>> street_mapping_;
    int bonus_;
};

/*

struct StringPointerHash {
    size_t operator()(const std::string* s) const {
        return std::hash<std::string>{}(*s);
    }
};

struct StringPointerEqual {
    bool operator()(const std::string* lhs, const std::string* rhs) const {
        return *lhs == *rhs;
    }
};

class SimulationShared {
public:
    //explicit SimulationShared(const std::string& filename);
    static SimulationShared from_file(const std::string& filename);

    SimulationShared(
        int duration,
        std::vector<IntersectionShared> intersections,
        std::vector<StreetShared> streets,
        std::vector<CarShared> cars,
        std::unordered_map<std::string_view, StreetShared*> street_mapping,
        //std::unordered_map<const std::string *, StreetShared *, StringPointerHash, StringPointerEqual> street_mapping,
        int bonus
    ) : duration_(duration), intersections_(std::move(intersections)), streets_(std::move(streets)), cars_(std::move(cars)), street_mapping_(std::move(street_mapping)), bonus_(bonus) {}

    int duration() const;

    const std::vector<IntersectionShared> &intersections() const;

    const std::vector<StreetShared> &streets() const;

    const std::vector<CarShared> &cars() const;

    int bonus() const;

    friend std::ostream& operator<<(std::ostream& os, const SimulationShared& obj);

private:
    const int duration_;
    const std::vector<IntersectionShared> intersections_;
    const std::vector<StreetShared> streets_;
    const std::vector<CarShared> cars_;
    const std::unordered_map<std::string_view, StreetShared*> street_mapping_;
    //const std::unordered_map<const std::string *, StreetShared*, StringPointerHash, StringPointerEqual> street_mapping_;
    const int bonus_;
};
*/

#endif
