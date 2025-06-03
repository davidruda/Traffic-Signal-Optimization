#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <optional>
#include <ranges>

#include "simulation/simulation.hpp"

namespace simulation {

Simulation::Simulation(const city_plan::CityPlan &city_plan) : city_plan_(city_plan) {
    streets_.reserve(city_plan_.streets().size());
    cars_.reserve(city_plan_.cars().size());

    for (auto &&s: city_plan_.streets()) {
        streets_.emplace_back(s);
    }
    for (auto &&c: city_plan_.cars()) {
        cars_.emplace_back(c);
    }
}

void Simulation::reset_run() {
    total_score_ = {};
    event_queue_ = {};
    for (auto &&s: streets_) {
        s.reset();
    }
    for (auto &&c: cars_) {
        c.reset();
    }
}

void Simulation::reset_schedules() {
    for (auto &&s: schedules_) {
        s.second.reset();
    }
    // Also reset information from possible previous runs
    reset_run();
}

void Simulation::load_schedules(const std::string &filename) {
    reset_schedules();
    std::ifstream file{filename};
    unsigned long number_of_intersections;
    unsigned long intersection_id;
    unsigned long number_of_streets;
    std::string street_name;
    unsigned long green_light_duration;

    file >> number_of_intersections;
    schedules_.reserve(number_of_intersections);

    for (auto i = 0UL; i < number_of_intersections; ++i) {
        file >> intersection_id >> number_of_streets;

        std::vector<unsigned long> order(number_of_streets), times(number_of_streets);
        for (auto j = 0UL; j < number_of_streets; ++j) {
            file >> street_name >> green_light_duration;
            order[j] = city_plan_.street_id(street_name);
            times[j] = green_light_duration;
        }

        if (schedules_.contains(intersection_id)) {
            schedules_.at(intersection_id).set(std::move(order), std::move(times));
            continue;
        }
        schedules_.try_emplace(
            intersection_id, city_plan_.intersections()[intersection_id], std::move(order), std::move(times)
        );
    }
}

void Simulation::save_schedules(const std::string &filename) const {
    std::ofstream file{filename};
    file << schedules_.size() << "\n";

    for (auto &&intersection: city_plan_.intersections()) {
        if (schedules_.contains(intersection.id())) {
            auto &&schedule = schedules_.at(intersection.id());
            file << intersection.id() << "\n";

            auto &&street_ids = schedule.order();
            auto &&times = schedule.times();
            file << times.size() << "\n";
            for (size_t i = 0; i < times.size(); ++i) {
                file << city_plan_.streets()[street_ids[i]].name() << " "
                     << times[i] << "\n";
            }
        }
    }
}

void Simulation::assign_schedules(Schedule::Order order_type, Schedule::Times times_type) {
    reset_schedules();
    for (auto &&intersection: city_plan_.used_intersections()) {
        auto &&[iter, _] = schedules_.try_emplace(intersection.id(), intersection);
        auto &&[id, schedule] = *iter;
        schedule.set(order_type, times_type);
    }
}

void Simulation::finalize_schedules(Schedule::Order order_type, Schedule::Times) {
    if (order_type == Schedule::Order::ADAPTIVE) {
        // When using adaptive schedules, the schedules are assigned while running the simulation
        // and the missing streets are filled in after the simulation is done
        run();
        reset_run();
        for (auto &&intersection: city_plan_.used_intersections()) {
            schedules_.at(intersection.id()).fill_missing_streets();
        }
    }
}

void Simulation::create_schedules(std::string order, std::string times, unsigned long divisor) {
    auto to_lower = [](auto c) {
        return static_cast<char>(std::tolower(c));
    };

    std::ranges::transform(order, order.begin(), to_lower);
    std::ranges::transform(times, times.begin(), to_lower);

    Schedule::Order order_type;
    Schedule::Times times_type;
    if (order == "default") {
        order_type = Schedule::Order::DEFAULT;
    }
    else if (order == "adaptive") {
        order_type = Schedule::Order::ADAPTIVE;
    }
    else if (order == "random") {
        order_type = Schedule::Order::RANDOM;
    }
    else {
        throw std::invalid_argument{"Invalid order option"};
    }

    if (times == "default") {
        times_type = Schedule::Times::DEFAULT;
    }
    else if (times == "scaled") {
        times_type = Schedule::Times::SCALED;
    }
    else {
        throw std::invalid_argument{"Invalid times option"};
    }

    if (order == "adaptive" && times != "default") {
        throw std::invalid_argument{"'adaptive' order can only be used with 'default' times"};
    }

    Schedule::set_divisor(divisor);
    assign_schedules(order_type, times_type);
    finalize_schedules(order_type, times_type);
}

void Simulation::initialize_run() {
    for (auto &&car: cars_) {
        add_event(car, 0);
    }
}

void Simulation::add_event(Car &car, unsigned long current_time) {
    auto street_id = car.current_street();
    auto intersection_id = city_plan_.streets()[street_id].end().id();

    // latest_used_time is the last time the street was used
    // (i.e. the last time a car passed through it)
    // It can be in the future (later that the current time)
    auto latest_used_time = streets_[street_id].latest_used_time();

    // earliest_possible_time is the theoretical next earliest time
    // the street can be used
    auto earliest_possible_time = current_time;
    if (latest_used_time.has_value()) {
        earliest_possible_time =
            (*latest_used_time >= current_time) ?
                *latest_used_time + 1 : current_time;
    }

    // If there's no schedule for the intersection, don't add the event
    if (!schedules_.contains(intersection_id)) {
        return;
    }
    auto next_green_time = schedules_.at(intersection_id).next_green(street_id, earliest_possible_time);

    // If the street has no scheduled green light, don't add the event
    if (!next_green_time.has_value()) {
        return;
    }
    streets_[street_id].add_car(car.id(), *next_green_time);
    event_queue_.emplace(*next_green_time, streets_[street_id]);
}

void Simulation::process_event() {
    auto current_time = event_queue_.top().time();
    auto &&car = cars_[event_queue_.top().street().get_car()];
    event_queue_.pop();

    car.move_to_next_street();
    auto street_id = car.current_street();
    auto street_length = city_plan_.streets()[street_id].length();

    // If car is at the last street in its path
    if (car.final_destination()) {
        auto finish_time = current_time + street_length;

        // If the car arrives at the end of the street before
        // the end of the simulation, mark it as finished and update the stats
        if (finish_time <= city_plan_.duration()) {
            car.arrive(finish_time, city_plan_.duration(), city_plan_.bonus());
            total_score_ += car.score();
        }
        return;
    }
    // Add event when the car arrives at the end of the street
    add_event(car, current_time + street_length);
}

void Simulation::run() {
    reset_run();
    initialize_run();

    while (!event_queue_.empty()) {
        if (event_queue_.top().time() > city_plan_.duration()) {
            break;
        }
        process_event();
    }
}

unsigned long Simulation::score() {
    run();
    return total_score_;
}

void Simulation::summary() const {
    unsigned long cars_finished = 0;
    unsigned long total_driving_time = 0;
    std::optional<std::reference_wrapper<const Car>> earliest_car;
    std::optional<std::reference_wrapper<const Car>> latest_car;
    for (auto &&c: cars_) {
        if (c.arrival_time()) {
            ++cars_finished;
            total_driving_time += *c.arrival_time();
            if (!earliest_car ||
                c.arrival_time() < earliest_car->get().arrival_time()) {
                earliest_car = c;
            }
            if (!latest_car ||
                c.arrival_time() > latest_car->get().arrival_time()) {
                latest_car = c;
            }
        }
    }

    double total_cycle_duration = 0;
    // All streets with a scheduled green light
    double total_green_streets = 0;
    unsigned long total_schedules = 0;
    for (auto &&[id, schedule]: schedules_) {
        if (schedule.length() == 0) {
            continue; // Skip empty schedules
        }
        total_cycle_duration += schedule.duration();
        total_green_streets += schedule.length();
        ++total_schedules;
    }
    auto average_cycle_length = total_cycle_duration / static_cast<double>(total_schedules);
    // auto average_cycle_length = total_cycle_duration / static_cast<double>(city_plan_.intersections().size());
    auto average_green_light_duration = total_cycle_duration / total_green_streets;

    // Ensure that the thousand separator is used for printing numbers
    // without relying on other locales such as en_US.UTF-8, which may not be available.
    std::locale custom_locale{std::locale{}, new ThousandSeparator};
    std::cout.imbue(custom_locale);

    std::cout
        << "The submission scored **" << total_score_ << " points**. "
        << "This is the sum of " << cars_finished * city_plan_.bonus()
        << " bonus points for cars arriving before the deadline ("
        << city_plan_.bonus() << " points each) and "
        << total_score_ - cars_finished * city_plan_.bonus()
        << " points for early arrival times.\n\n"
        << cars_finished << " of " << city_plan_.cars().size()
        << " cars arrived before the deadline (";
    auto finished_percentage = cars_finished / static_cast<double>(city_plan_.cars().size()) * 100;
    std::cout
        << std::fixed << std::setprecision(2) << finished_percentage << "%). ";

    if (earliest_car && latest_car) {
        auto average_drive_time_ = static_cast<double>(total_driving_time) / cars_finished;

        std::cout
            << "The earliest car (ID " << earliest_car->get().id()
            << ") arrived at its destination after "
            << *earliest_car->get().arrival_time() << " seconds scoring "
            << earliest_car->get().score() << " points, whereas the last car (ID "
            << latest_car->get().id() << ") arrived at its destination after "
            << *latest_car->get().arrival_time() << " seconds scoring "
            << latest_car->get().score() << " points. "
            << "Cars that arrived within the deadline drove for an average of "
            << average_drive_time_ << " seconds to arrive at their destination.";

        auto scheduled_percentage = total_schedules / static_cast<double>(city_plan_.intersections().size()) * 100;

        std::cout
            << "\n\n"
            << "The schedules for the " << total_schedules
            << " traffic lights, out of " << city_plan_.intersections().size()
            << " traffic lights in total (" << std::fixed << std::setprecision(2) << scheduled_percentage
            << "%), had an average total cycle length of "
            << std::fixed << std::setprecision(2) << average_cycle_length
            << " seconds. A traffic light that turned green was scheduled to stay green for "
            << std::fixed << std::setprecision(2) << average_green_light_duration
            << " seconds on average."
            << "\n";
    }
}

std::vector<std::pair<std::vector<unsigned long>, std::vector<unsigned long>>> Simulation::non_trivial_schedules(
    bool relative_order
) const {
    if (schedules_.empty()) {
        return {};
    }

    auto &&non_trivial_schedules_view = city_plan_.non_trivial_intersections()
        | std::views::transform(&city_plan::Intersection::id)
        | std::views::transform([&](unsigned long intersection_id) {
            return schedules_.at(intersection_id);
        })
        | std::views::transform([&](const Schedule &schedule) {
            if (relative_order) {
                auto &&rl_view = schedule.relative_order();
                auto &&rl = std::vector(rl_view.begin(), rl_view.end());
                return std::pair{rl, schedule.times()};
            }
            return std::pair{schedule.order(), schedule.times()};
        });

        return {non_trivial_schedules_view.begin(), non_trivial_schedules_view.end()};
}

// TODO: maybe rewrite this function specifically for python types
// relative_order - whether the order is of indices relative to the intersection
// or absolute street ids
void Simulation::set_non_trivial_schedules(
    std::vector<std::pair<std::vector<unsigned long>, std::vector<unsigned long>>> &&schedules,
    bool relative_order
) {
    // IMPORTANT: Note that schedules must have the same order and size as non_trivial_intersections
    size_t i = 0;
    for (auto &&intersection: city_plan_.non_trivial_intersections()) {
        auto &&[order, times] = schedules[i++];

        // TODO: maybe solve the relative/absolute street id problem differently
        if (relative_order) {
            auto &&used_streets = intersection.used_streets();
            auto street_ids = order | std::views::transform([&](unsigned long street_index) {
                return static_cast<const city_plan::Street &>(used_streets[street_index]).id();
            });
            order = {street_ids.begin(), street_ids.end()};
        }
        schedules_.at(intersection.id()).set(std::move(order), std::move(times));
    }
}

Simulation default_simulation(const city_plan::CityPlan &city_plan) {
    Simulation s{city_plan};
    s.default_schedules();
    return s;
}

Simulation adaptive_simulation(const city_plan::CityPlan &city_plan) {
    Simulation s{city_plan};
    s.adaptive_schedules();
    return s;
}
}
