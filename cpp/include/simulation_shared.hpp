#ifndef SIMULATION_SHARED_HPP
#define SIMULATION_SHARED_HPP

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "car_shared.hpp"
#include "intersection_shared.hpp"
#include "street_shared.hpp"


class SimulationShared {
public:
    explicit SimulationShared(const std::string &filename);

    const std::vector<IntersectionShared> &intersections() const;
    const std::vector<StreetShared> &streets() const;
    const std::vector<CarShared> &cars() const;
    int duration() const;
    int bonus() const;

    friend std::ostream &operator<<(std::ostream &os, const SimulationShared &obj);

private:
    int duration_;
    std::vector<IntersectionShared> intersections_;
    std::vector<StreetShared> streets_;
    std::vector<CarShared> cars_;
    std::unordered_map<std::string_view, std::reference_wrapper<StreetShared>> street_mapping_;
    int bonus_;
};

#endif
