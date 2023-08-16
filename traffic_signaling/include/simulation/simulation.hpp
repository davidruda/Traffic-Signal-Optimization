#ifndef SIMULATION_SIMULATION_HPP
#define SIMULATION_SIMULATION_HPP

#include <functional>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "car.hpp"
#include "city_plan/city_plan.hpp"
#include "event.hpp"
#include "intersection.hpp"
#include "schedule.hpp"
#include "street.hpp"

namespace simulation {

    class Simulation {
    public:
        explicit Simulation(const city_plan::CityPlan &city_plan);
        Simulation(const Simulation &other) = delete;
        Simulation &operator=(const Simulation &other) = delete;

        void read_plan(const std::string &filename);
        void write_plan(const std::string &filename);

        // default means 1 second for every used street in the given order
        void create_plan_default();

        Simulation &run();
        size_t score() const;
        void summary() const;

        const std::vector<Intersection> &intersections() const {
            return intersections_;
        }

        const std::vector<Street> &streets() const {
            return streets_;
        }

        const std::vector<Car> &cars() const {
            return cars_;
        }

        std::unordered_map<size_t, Schedule> &schedules() {
            return schedules_;
        }

    private:
        void reset_run();
        void reset_plan();
        void add_car_event(Car &car, size_t time);
        void add_street_event(Street &street, size_t time);
        void initialize_run();
        void process_car_events();
        void process_street_events();

        const city_plan::CityPlan &city_plan_;

        std::vector<Intersection> intersections_;
        std::vector<Street> streets_;
        std::vector<Car> cars_;
        std::unordered_map<size_t, Schedule> schedules_;

        std::priority_queue<CarEvent, std::vector<CarEvent>, std::greater<>> car_events_;
        std::priority_queue<StreetEvent, std::vector<StreetEvent>, std::greater<>> street_events_;
    };
}

#endif
