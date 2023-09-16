#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string_view>

#include "simulation/simulation.hpp"

namespace simulation {

Simulation::Simulation(const city_plan::CityPlan &city_plan)
    : city_plan_(city_plan) {
    intersections_.reserve(city_plan_.intersections().size());
    streets_.reserve(city_plan_.streets().size());
    cars_.reserve(city_plan_.cars().size());

    for (auto &&i: city_plan_.intersections()) {
        intersections_.emplace_back(i);
    }
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
    for (auto &&i: intersections_) {
        i.reset();
    }
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
}

void Simulation::read_schedules(const std::string &filename) {
    reset_schedules();
    std::ifstream file{filename};
    size_t number_of_intersections;
    size_t intersection_id;
    size_t number_of_streets;
    std::string street_name;
    size_t green_light_duration;

    std::unordered_map<std::string_view, size_t> street_mapping;
    street_mapping.reserve(streets_.size());
    for (auto &&s: streets_) {
        street_mapping.emplace(s.name(), s.id());
    }

    file >> number_of_intersections;
    schedules_.reserve(number_of_intersections);

    for (size_t i = 0; i < number_of_intersections; ++i) {
        file >> intersection_id;

        file >> number_of_streets;
        for (size_t j = 0; j < number_of_streets; ++j) {
            file >> street_name >> green_light_duration;
            auto &&street_id = street_mapping[street_name];
            schedules_[intersection_id].add_street(street_id, green_light_duration);
        }
    }
}

void Simulation::write_schedules(const std::string &filename) const {
    std::ofstream file{filename};
    file << schedules_.size() << "\n";

    for (auto &&intersection: intersections_) {
        if (schedules_.contains(intersection.id())) {
            auto &&schedule = schedules_.at(intersection.id());
            file << intersection.id() << "\n"
                 << schedule.length() << "\n";

            for (auto &&[street_id, green_light]: schedule.green_lights()) {
                file << streets_[street_id].name() << " " << green_light.duration() << "\n";
            }
        }
    }
}

void Simulation::default_schedules() {
    reset_schedules();
    for (auto &&intersection: intersections_) {
        for (auto &&s: intersection.incoming_streets()) {
            auto &&street = streets_[s];
            if (street.used()) {
                schedules_[intersection.id()].add_street(street.id(), 1);
            }
        }
    }
}

void Simulation::initialize_run() {
    for (auto &&car: cars_) {
        add_event(car, 0);
    }
}

void Simulation::add_event(Car &car, size_t current_time) {
    auto &&street = streets_[car.current_street()];
    auto &&intersection = intersections_[street.end()];

    // latest_used_time is the last time the street was used
    // (i.e. the last time a car passed through it)
    // It can be in the future (later that the current time)
    auto latest_used_time = street.latest_used_time();

    // earliest_possible_time is the theoretical next earliest time
    // the street can be used
    size_t earliest_possible_time = current_time;
    if (latest_used_time.has_value()) {
        earliest_possible_time = (*latest_used_time >= current_time) ? *latest_used_time + 1 : current_time;
    }

    // If there's no schedule for the intersection, don't add the event
    if (!schedules_.contains(intersection.id())) {
        return;
    }
    auto next_green_time = schedules_[intersection.id()].next_green(street.id(), earliest_possible_time);

    // If the street has no scheduled green light, don't add the event
    if (!next_green_time.has_value()) {
        return;
    }
    street.add_car(car.id(), *next_green_time);
    event_queue_.emplace(*next_green_time, street);
}

void Simulation::process_event() {
    auto current_time = event_queue_.top().time();
    auto &&car = cars_[event_queue_.top().street().get_car()];
    event_queue_.pop();

    car.move_to_next_street();
    auto &&street = streets_[car.current_street()];

    // If car is at the last street in its path
    if (car.final_destination()) {
        auto finish_time = current_time + street.length();

        // If the car arrives at the end of the street before
        // the end of the simulation, mark it as finished and update the stats
        if (finish_time <= city_plan_.duration()) {
            car.set_finish_time(finish_time);
            total_score_ +=
                    city_plan_.bonus() + city_plan_.duration() - finish_time;
        }
        return;
    }
    // Add event when the car arrives at the end of the street
    add_event(car, current_time + street.length());
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

size_t Simulation::score() {
    run();
    return total_score_;
}

void Simulation::summary() const {
    Statistics stats;
    for (auto &&c: cars_) {
        if (c.finish_time()) {
            ++stats.cars_finished_;
            stats.total_driving_time_ += *c.finish_time();
            auto car_score =
                    city_plan_.bonus() + city_plan_.duration() - *c.finish_time();
            if (car_score > stats.max_car_score_) {
                stats.max_car_score_ = car_score;
                stats.earliest_car_ = c;
            }
            if (car_score < stats.min_car_score_) {
                stats.min_car_score_ = car_score;
                stats.latest_car_ = c;
            }
        }
    }
    print_summary(stats);
}

void Simulation::print_summary(const Simulation::Statistics &stats) const {
    // Ensure that the thousand separator is used for printing numbers
    // without relying on other locales such as en_US.UTF-8, which may not be available.
    std::locale custom_locale{std::locale{}, new ThousandSeparator};
    std::cout.imbue(custom_locale);

    std::cout
        << "The submission scored **" << total_score_ << " points**. "
        << "This is the sum of " << stats.cars_finished_ * city_plan_.bonus()
        << " bonus points for cars arriving before the deadline ("
        << city_plan_.bonus() << " points each) and "
        << total_score_ - stats.cars_finished_ * city_plan_.bonus()
        << " points for early arrival times.\n\n"
        << stats.cars_finished_ << " of "
        << city_plan_.cars().size() << " cars arrived before the deadline (";
    auto finished_percentage =
        static_cast<float>(stats.cars_finished_) /
        static_cast<float>(city_plan_.cars().size()) * 100;
    std::cout
        << std::fixed << std::setprecision(2) << finished_percentage << "%). ";

    if (stats.earliest_car_ && stats.latest_car_) {
        auto average_drive_time_ =
            static_cast<float>(stats.total_driving_time_) /
            static_cast<float>(stats.cars_finished_);

        std::cout
            << "The earliest car (ID " << stats.earliest_car_->get().id()
            << ") arrived at its destination after "
            << *stats.earliest_car_->get().finish_time() << " seconds scoring "
            << stats.max_car_score_ << " points, whereas the last car (ID "
            << stats.latest_car_->get().id() << ") arrived at its destination after "
            << *stats.latest_car_->get().finish_time() << " seconds scoring "
            << stats.min_car_score_ << " points. "
            << "Cars that arrived within the deadline drove for an average of "
            << average_drive_time_ << " seconds to arrive at their destination.";
    }
    std::cout << "\n";
}

void Simulation::update_schedules(const std::unordered_map<size_t,
                                                           std::pair<std::vector<size_t>, std::vector<size_t>>> &schedules) {
    for (auto &&[id, schedule_pair]: schedules) {
        auto &&[times, order] = schedule_pair;
        schedules_[id].set_schedule(times, order);
    }
}
}
