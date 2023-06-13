#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "car.hpp"
#include "event.hpp"
#include "intersection.hpp"
#include "schedule.hpp"
#include "street.hpp"

class Simulation {
public:
    explicit Simulation(const std::string &filename);

    const std::vector<Intersection> &intersections() const;
    const std::vector<Street> &streets() const;
    const std::vector<Car> &cars() const;
    size_t duration() const;
    size_t bonus() const;

    friend std::ostream &operator<<(std::ostream &os, const Simulation &obj);

    class Instance;

    Instance create_instance();

private:
    size_t duration_;
    std::vector<Intersection> intersections_;
    std::vector<Street> streets_;
    std::vector<Car> cars_;
    std::unordered_map<std::string_view, size_t> street_mapping_;
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

private:
    void reset_run();
    void reset_plan();
    void initialize_event_queue(auto &event_queue);
    void process_street_event(auto &event_queue, auto &event);
    void process_car_event(auto &event_queue, auto &event);

    const Simulation &data_;

    std::vector<Intersection::Instance> intersections_;
    std::vector<Street::Instance> streets_;
    std::vector<Car::Instance> cars_;
    std::unordered_map<size_t, Schedule> schedules_;
};

#endif
