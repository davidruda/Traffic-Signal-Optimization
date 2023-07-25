#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <memory>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "city_plan.hpp"
#include "event.hpp"
#include "schedule.hpp"
#include "simulation_car.hpp"
#include "simulation_intersection.hpp"
#include "simulation_street.hpp"

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

    private:
        void reset_run();
        void reset_plan();
        void initialize_event_queue();
        void process_street_event(const std::unique_ptr<Event> &event);
        void process_car_event(const std::unique_ptr<Event> &event);

        const city_plan::CityPlan &city_plan_;

        std::vector<Intersection> intersections_;
        std::vector<Street> streets_;
        std::vector<Car> cars_;
        std::unordered_map<size_t, Schedule> schedules_;

        struct EventComparator {
            // an earlier event has higher priority
            bool operator()(const std::unique_ptr<Event> &lhs, const std::unique_ptr<Event> &rhs) {
                return !(*lhs < *rhs);
            };
        };

        std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, EventComparator> event_queue_;
    };
}

#endif
