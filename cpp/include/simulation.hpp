#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "car.hpp"
#include "intersection.hpp"
#include "street.hpp"


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

class SimulationInstance {
public:
    explicit SimulationInstance(const SimulationShared &data);

    void run();
    int score(bool verbose) const;

    const std::vector<IntersectionInstance> &intersections() const;
    const std::vector<StreetInstance> &streets() const;
    const std::vector<CarInstance> &cars() const;

private:
    const SimulationShared &data_;

    std::vector<IntersectionInstance> intersections_;
    std::vector<StreetInstance> streets_;
    std::vector<CarInstance> cars_;
};

#endif
