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

class Simulation {
public:
    explicit Simulation(const std::string &filename);

    const std::vector<Intersection> &intersections() const;
    const std::vector<Street> &streets() const;
    const std::vector<Car> &cars() const;
    size_t duration() const;
    size_t bonus() const;
    const Street &street_by_name(const std::string &name) const;

    friend std::ostream &operator<<(std::ostream &os, const Simulation &obj);

    class Instance;

    Instance create_instance();

private:
    size_t duration_;
    std::vector<Intersection> intersections_;
    std::vector<Street> streets_;
    std::vector<Car> cars_;
    std::unordered_map<std::string_view, std::reference_wrapper<Street>> street_mapping_;
    size_t bonus_;
};

class Simulation::Instance {
public:
    explicit Instance(const Simulation &data);

    void read_plan(const std::string &filename);
    void write_plan(const std::string &filename);

    // default means 1 second for every used street in the given order
    void create_plan_default();

    Instance &run();
    size_t score(bool verbose = false) const;

    const std::vector<Intersection::Instance> &intersections() const;
    const std::vector<Street::Instance> &streets() const;
    const std::vector<Car::Instance> &cars() const;
    const Street &street_by_name(const std::string &name) const;


private:
    const Simulation &data_;

    std::vector<Intersection::Instance> intersections_;
    std::vector<Street::Instance> streets_;
    std::vector<Car::Instance> cars_;
};

#endif
