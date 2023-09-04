#ifndef SIMULATION_SIMULATION_HPP
#define SIMULATION_SIMULATION_HPP

#include <functional>
#include <locale>
#include <optional>
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

        void read_plan(const std::string &filename);
        void write_plan(const std::string &filename);

        // default means 1 second for every used street in the given order
        void create_plan_default();

        size_t score(bool verbose = false);

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
        struct Statistics {
            Statistics() = default;
            explicit Statistics(bool verbose) : verbose_(verbose) {}

            static void reset(Simulation &simulation, bool verbose);
            void update(const city_plan::CityPlan &city_plan, const Car &car);
            void summary(const city_plan::CityPlan &city_plan);

            bool verbose_{};
            size_t total_score_{};
            size_t cars_finished_{};
            size_t total_driving_time_{};
            size_t max_car_score_{};
            size_t min_car_score_{std::numeric_limits<size_t>::max()};
            float average_drive_time_{};
            std::optional<std::reference_wrapper<const Car>> earliest_car_;
            std::optional<std::reference_wrapper<const Car>> latest_car_;
        };

        class ThousandSeparator : public std::numpunct<char> {
        protected:
            // Use a comma as the thousand separator
            char do_thousands_sep() const override {
                return ',';
            }

            // Use groups of 3 digits for the thousand separator
            std::string do_grouping() const override {
                return "\03";
            }
        };

        void run();
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

        Statistics stats_;
    };
}

#endif
