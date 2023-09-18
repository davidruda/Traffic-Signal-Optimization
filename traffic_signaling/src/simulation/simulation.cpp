#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>


#include "simulation/simulation.hpp"

namespace simulation {

Simulation::Simulation(const city_plan::CityPlan &city_plan)
    : city_plan_(city_plan) {
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
}

void Simulation::read_schedules(const std::string &filename) {
    reset_schedules();
    std::ifstream file{filename};
    size_t number_of_intersections;
    size_t intersection_id;
    size_t number_of_streets;
    std::string street_name;
    size_t green_light_duration;

    file >> number_of_intersections;
    schedules_.reserve(number_of_intersections);

    for (size_t i = 0; i < number_of_intersections; ++i) {
        file >> intersection_id;

        file >> number_of_streets;
        for (size_t j = 0; j < number_of_streets; ++j) {
            file >> street_name >> green_light_duration;
            auto &&street_id = city_plan_.street_mapping().at(street_name);
            schedules_[intersection_id].add_street(street_id, green_light_duration);
        }
    }
}

void Simulation::write_schedules(const std::string &filename) const {
    std::ofstream file{filename};
    file << schedules_.size() << "\n";

    for (auto &&intersection: city_plan_.intersections()) {
        if (schedules_.contains(intersection.id())) {
            auto &&schedule = schedules_.at(intersection.id());
            file << intersection.id() << "\n"
                 << schedule.length() << "\n";

            for (auto &&[street_id, green_light]: schedule.green_lights()) {
                file << city_plan_.streets()[street_id].name() << " "
                     << green_light.duration() << "\n";
            }
        }
    }
}

void Simulation::default_schedules() {
    reset_schedules();
    for (auto &&intersection: city_plan_.intersections()) {
        if (intersection.used()) {
            for (auto &&s: intersection.incoming_streets()) {
                auto &&street = city_plan_.streets()[s];
                if (street.used()) {
                    schedules_[intersection.id()].add_street(street.id(), 1);
                }
            }
        }
    }
}
/*
void Simulation::scaled_schedules() {
    reset_schedules();
    std::unordered_map<size_t, size_t> car_counts;
    for (auto &&car: city_plan_.cars()) {
        // size - 1 because the car doesn't need to wait for
        // a green light at the last street
        for (size_t i = 0; i < car.path().size() - 1; ++i) {
            auto street_id = car.path()[i];
            car_counts[street_id] += 1;
        }
    }

    for (auto &&intersection: city_plan_.intersections()) {
        if (intersection.non_trivial()) {
            std::unordered_map<size_t, size_t> times;
            for (auto &&street_id: intersection.incoming_streets()) {
                if (city_plan_.streets()[street_id].used()) {
                    times.emplace(street_id, car_counts[street_id]);
                }
            }
            auto cmp = [](const auto &lhs, const auto &rhs) {
                return lhs.second < rhs.second;
            };
            auto min_time = std::min_element(times.begin(), times.end(), cmp)->second;
            for (auto &&[street_id, time]: times) {

                auto scaled_time = static_cast<size_t>(std::round(static_cast<float>(time) / static_cast<float>(min_time)));
                schedules_[intersection.id()].add_street(street_id, scaled_time);
            }
        }
        else if (intersection.used()) {
            for (auto &&street_id: intersection.incoming_streets()) {
                auto &&street = city_plan_.streets()[street_id];
                if (street.used()) {
                    // If the intersection is used but not non-trivial,
                    // it must have only one used incoming street
                    schedules_[intersection.id()].add_street(street_id, 1);
                    break;
                }
            }
        }
    }
}*/

void Simulation::initialize_run() {
    for (auto &&car: cars_) {
        add_event(car, 0);
    }
}

void Simulation::add_event(Car &car, size_t current_time) {
    auto street_id = car.current_street();
    auto intersection_id = city_plan_.streets()[street_id].end();

    // latest_used_time is the last time the street was used
    // (i.e. the last time a car passed through it)
    // It can be in the future (later that the current time)
    auto latest_used_time = streets_[street_id].latest_used_time();

    // earliest_possible_time is the theoretical next earliest time
    // the street can be used
    size_t earliest_possible_time = current_time;
    if (latest_used_time.has_value()) {
        earliest_possible_time =
            (*latest_used_time >= current_time) ?
                *latest_used_time + 1 : current_time;
    }

    // If there's no schedule for the intersection, don't add the event
    if (!schedules_.contains(intersection_id)) {
        return;
    }
    auto next_green_time = schedules_[intersection_id].next_green(street_id, earliest_possible_time);

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
            car.arrive(city_plan_, finish_time);
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

size_t Simulation::score() {
    run();
    return total_score_;
}

void Simulation::summary() const {
    size_t cars_finished = 0;
    size_t total_driving_time = 0;
    std::optional<std::reference_wrapper<const Car>> earliest_car;
    std::optional<std::reference_wrapper<const Car>> latest_car;
    for (auto &&c: cars_) {
        if (c.arrival_time()) {
            ++cars_finished;
            total_driving_time += *c.arrival_time();
            if (!earliest_car ||
                c.arrival_time() > earliest_car->get().arrival_time()) {
                earliest_car = c;
            }
            if (!latest_car ||
                c.arrival_time() < latest_car->get().arrival_time()) {
                latest_car = c;
            }
        }
    }
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
    auto finished_percentage =
        static_cast<float>(cars_finished) /
        static_cast<float>(city_plan_.cars().size()) * 100;
    std::cout
        << std::fixed << std::setprecision(2) << finished_percentage << "%). ";

    if (earliest_car && latest_car) {
        auto average_drive_time_ =
            static_cast<float>(total_driving_time) /
            static_cast<float>(cars_finished);

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
