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
/**
 * Simulation of the Traffic signaling problem for a given city plan.
 */
class Simulation {
public:
    /**
     * Construct a simulation object for the given city plan.
     *
     * @param city_plan City plan containing information from the input file.
     */
    explicit Simulation(const city_plan::CityPlan &city_plan);

    /**
     * Load and create schedules from the given file.
     *
     * @param filename Path of the file containing schedules in the competition format.
     */
    void load_schedules(const std::string &filename);

    /**
     * Save the current schedules in the competition format to the given file.
     *
     * @param filename Path of the file to save the schedules to.
     */
    void save_schedules(const std::string &filename) const;

    /**
     * Create schedules for all used intersections and used streets.
     *
     * @param order Initialization option for the order of streets in the schedules.
     * @param times Initialization option for the green light times in the schedules.
     * @param divisor Divisor for the scaled times option.
     */
    void create_schedules(std::string order, std::string times, unsigned long divisor = Schedule::DEFAULT_DIVISOR);

    /**
     * Create default schedules.
     *
     * This is an alias for `create_schedules("default", "default")`.
     */
    void default_schedules() {
        // default means 1 second for every used street in the default order
        create_schedules("default", "default");
    }

    /**
     * Create adaptive schedules.
     *
     * This is an alias for `create_schedules("adaptive", "default")`.
     */
    void adaptive_schedules() {
        create_schedules("adaptive", "default");
    }

    /**
     * Create random schedules.
     *
     * This is an alias for `create_schedules("random", "default")`.
     */
    void random_schedules() {
        create_schedules("random", "default");
    }

    /**
     * Create scaled schedules.
     *
     * This is an alias for `create_schedules("default", "scaled", divisor)`.
     *
     * @param divisor Divisor for the scaled times option.
     */
    void scaled_schedules(unsigned long divisor = Schedule::DEFAULT_DIVISOR) {
        create_schedules("default", "scaled", divisor);
    }

    /**
     * Calculate the score for the current setting of schedules.
     *
     * This method runs the simulation.
     */
    unsigned long score();

    /**
     * Print a summary of the simulation statistics.
     */
    void summary() const;

    /**
     * Return the current schedules as a map indexed by intersection IDs.
     */
    const std::unordered_map<unsigned long, Schedule> &schedules() const {
        return schedules_;
    }

    /**
     * Return the schedules of non-trivial intersections as a vector of `(order, times)` pairs,
     * where each pair is a schedule for one intersection.
     *
     * @param relative_order If True, `order` is a vector of street indices relative to each intersection.
     * Otherwise, `order` is a vector of street IDs.
     */
    std::vector<std::pair<std::vector<unsigned long>, std::vector<unsigned long>>> non_trivial_schedules(
        bool relative_order = false
    ) const;

    /**
     *
     * @param schedules Vector of `(order, times)` pairs, where each pair is a schedule for one intersection.
     * @param relative_order If True, `order` must be a vector of street indices relative to each intersection.
     * Otherwise, `order` must be a vector of street IDs.
     *
     * This method assumes the input is valid and performs no validation.
     */
    void set_non_trivial_schedules(
        std::vector<std::pair<std::vector<unsigned long>, std::vector<unsigned long>>> &&schedules,
        bool relative_order = false
    );

private:
    /** Custom separator to ensure correct thousand separators in the output. */
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

    /**
     * Run the simulation.
     */
    void run();

    /**
     * Reset the simulation run state.
     */
    void reset_run();

    /**
     * Reset the schedules to their initial state.
     */
    void reset_schedules();

    /**
     * Assign schedules for all used intersections based on the given order and times initialization options.
     *
     * @param order_type Order initialization option for the schedules.
     * @param times_type Times initialization option for the schedules.
     */
    void assign_schedules(Schedule::Order order_type, Schedule::Times times_type);

    /**
     * Finalize the schedules after initial assignment.
     *
     * This method fills in the not yet assigned streets for adaptive schedules.
     *
     * @param order_type Order initialization option for the schedules.
     * @param times_type Times initialization option for the schedules.
     */
    void finalize_schedules(Schedule::Order order_type, Schedule::Times times_type);

    /**
     * Add a `StreetEvent` for the given car to the event queue.
     *
     * Schedules a new event for the car based on the current simulation time.
     *
     * @param car The car for which to add the event.
     * @param current_time Current time in the simulation.
     */
    void add_event(Car &car, unsigned long current_time);

    /**
     * Initialize the run state of the simulation.
     */
    void initialize_run();

    /**
     * Process the next `StreetEvent` in the event queue.
     *
     * This method processes the event at the top of the event queue, updating the state of the simulation
     * and scheduling new events as necessary.
     */
    void process_event();

    /** City plan containing all information from the input file. */
    const city_plan::CityPlan &city_plan_;

    /** Streets in the simulation. */
    std::vector<Street> streets_;
    /** Cars in the simulation. */
    std::vector<Car> cars_;
    /** Schedules for intersections indexed by intersection IDs. */
    std::unordered_map<unsigned long, Schedule> schedules_;

    /**
     * Event queue for the simulation, containing `StreetEvent` objects.
     */
    std::priority_queue<StreetEvent, std::vector<StreetEvent>, std::greater<>> event_queue_;

    /** Score of the last simulation run. */
    unsigned long total_score_{};
};

/**
 * Create a simulation with default schedules for the given city plan.
 *
 * This is a shortcut for creating a simulation object and calling its `default_schedules` method.
 *
 * @param city_plan City plan containing information from the input file.
 */
Simulation default_simulation(const city_plan::CityPlan &city_plan);

/**
 * Create a simulation with adaptive schedules for the given city plan.
 *
 * This is a shortcut for creating a simulation object and calling its `adaptive_schedules` method.
 *
 * @param city_plan City plan containing information from the input file.
 */
Simulation adaptive_simulation(const city_plan::CityPlan &city_plan);
}

#endif
