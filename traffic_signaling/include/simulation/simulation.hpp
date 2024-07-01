#ifndef SIMULATION_SIMULATION_HPP
#define SIMULATION_SIMULATION_HPP

#include <functional>
#include <locale>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "city_plan/city_plan.hpp"
#include "simulation/car.hpp"
#include "simulation/event.hpp"
#include "simulation/schedule.hpp"
#include "simulation/street.hpp"

namespace simulation {
class Simulation {
public:
    explicit Simulation(const city_plan::CityPlan &city_plan);

    void load_schedules(const std::string &filename);
    void save_schedules(const std::string &filename) const;

    void create_schedules(std::string order, std::string times, unsigned long divisor = Schedule::DEFAULT_DIVISOR);

    // default means 1 second for every used street in the given order
    void default_schedules() {
        create_schedules("default", "default");
    }

    void adaptive_schedules() {
        create_schedules("adaptive", "default");
    }

    void random_schedules() {
        create_schedules("random", "default");
    }

    void scaled_schedules(unsigned long divisor = Schedule::DEFAULT_DIVISOR) {
        create_schedules("default", "scaled", divisor);
    }

    unsigned long score();
    void summary() const;

    const std::unordered_map<unsigned long, Schedule> &schedules() const {
        return schedules_;
    }

    std::vector<std::pair<std::vector<unsigned long>, std::vector<unsigned long>>> non_trivial_schedules(
        bool relative_order = false
    ) const;

    void set_non_trivial_schedules(
        std::vector<std::pair<std::vector<unsigned long>, std::vector<unsigned long>>> &&schedules,
        bool relative_order = false
    );

private:
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
    void reset_schedules();
    void assign_schedules(Schedule::Order order_type, Schedule::Times times_type);
    void finalize_schedules(Schedule::Order order_type, Schedule::Times times_type);
    void add_event(Car &car, unsigned long current_time);
    void initialize_run();
    void process_event();

    const city_plan::CityPlan &city_plan_;

    std::vector<Street> streets_;
    std::vector<Car> cars_;
    std::unordered_map<unsigned long, Schedule> schedules_;

    std::priority_queue<StreetEvent, std::vector<StreetEvent>, std::greater<>> event_queue_;

    unsigned long total_score_{};
};

// factory function creating a simulation with default schedules
Simulation default_simulation(const city_plan::CityPlan &city_plan);
// factory function creating a simulation with adaptive schedules
Simulation adaptive_simulation(const city_plan::CityPlan &city_plan);
}

#endif
