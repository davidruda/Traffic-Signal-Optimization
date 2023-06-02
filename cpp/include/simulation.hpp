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

    const std::vector<Intersection> &intersections() const;
    const std::vector<Street> &streets() const;
    const std::vector<Car> &cars() const;
    int duration() const;
    int bonus() const;

    friend std::ostream &operator<<(std::ostream &os, const SimulationShared &obj);

private:
    int duration_;
    std::vector<Intersection> intersections_;
    std::vector<Street> streets_;
    std::vector<Car> cars_;
    std::unordered_map<std::string_view, std::reference_wrapper<Street>> street_mapping_;
    int bonus_;
};

class SimulationInstance {
public:
    explicit SimulationInstance(const SimulationShared &data);

    void run();
    int score(bool verbose) const;

    const std::vector<Intersection::Instance> &intersections() const;
    const std::vector<Street::Instance> &streets() const;
    const std::vector<Car::Instance> &cars() const;

private:
    const SimulationShared &data_;

    std::vector<Intersection::Instance> intersections_;
    std::vector<Street::Instance> streets_;
    std::vector<Car::Instance> cars_;
};

#endif
