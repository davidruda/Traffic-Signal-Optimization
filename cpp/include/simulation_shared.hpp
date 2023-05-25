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

    const std::vector<IntersectionShared> &intersections() const;
    const std::vector<StreetShared> &streets() const;
    const std::vector<CarShared> &cars() const;

    friend std::ostream& operator<<(std::ostream& os, const SimulationShared& obj);

private:
    int duration_;
    std::vector<IntersectionShared> intersections_;
    std::vector<StreetShared> streets_;
    std::vector<CarShared> cars_;
    std::unordered_map<std::string_view, std::reference_wrapper<StreetShared>> street_mapping_;
    int bonus_;
};

#endif
