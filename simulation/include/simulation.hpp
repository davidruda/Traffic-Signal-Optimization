#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "city_plan.hpp"
#include "schedule.hpp"
#include "simulation_car.hpp"
#include "simulation_intersection.hpp"
#include "simulation_street.hpp"

class Simulation {
public:
    explicit Simulation(const CityPlan &city_plan);

    void read_plan(const std::string &filename);
    void write_plan(const std::string &filename);

    // default means 1 second for every used street in the given order
    void create_plan_default();

    Simulation &run();
    size_t score() const;
    void summary() const;

    const std::vector<SimulationIntersection> &intersections() const;
    const std::vector<SimulationStreet> &streets() const;
    const std::vector<SimulationCar> &cars() const;

private:
    void reset_run();
    void reset_plan();
    void initialize_event_queue(auto &event_queue);
    void process_street_event(auto &event_queue, auto &event);
    void process_car_event(auto &event_queue, auto &event);

    const CityPlan &city_plan_;

    std::vector<SimulationIntersection> intersections_;
    std::vector<SimulationStreet> streets_;
    std::vector<SimulationCar> cars_;
    std::unordered_map<size_t, Schedule> schedules_;
};

#endif
