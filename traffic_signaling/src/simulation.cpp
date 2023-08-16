#include <algorithm>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>

#include "simulation.hpp"

namespace simulation {

    Simulation::Simulation(const city_plan::CityPlan &city_plan) : city_plan_(city_plan) {
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
        car_events_ = {};
        street_events_ = {};
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

    void Simulation::reset_plan() {
        for (auto &&s: schedules_) {
            s.second.reset();
        }
    }

    void Simulation::read_plan(const std::string &filename) {
        reset_plan();
        std::ifstream file{filename};
        size_t number_of_intersections;
        size_t intersection_id;
        size_t number_of_streets;
        std::string street_name;
        size_t green_light_duration;

        file >> number_of_intersections;
        for (size_t i = 0; i < number_of_intersections; ++i) {
            file >> intersection_id;

            file >> number_of_streets;
            for (size_t j = 0; j < number_of_streets; ++j) {
                file >> street_name >> green_light_duration;
                auto &&street_id = city_plan_.get_street_id_by_name(street_name);
                schedules_[intersection_id].add_street(street_id, green_light_duration);
            }
        }
    }

    void Simulation::write_plan(const std::string &filename) {
        std::ofstream file{filename};
        file << schedules_.size() << "\n";

        for (auto &&intersection: intersections_) {
            if (schedules_.contains(intersection.id())) {
                auto &&schedule = schedules_[intersection.id()];
                file << intersection.id() << "\n"
                     << schedule.length() << "\n";

                for (auto &&[street_id, green_light]: schedule.green_light_schedule()) {
                    file << streets_[street_id].name() << " " << green_light.duration() << "\n";
                }
            }
        }
    }

    void Simulation::create_plan_default() {
        reset_plan();
        for (auto &&intersection: intersections_) {
            for (auto &&s: intersection.incoming()) {
                auto &&street = streets_[s];
                if (street.is_used()) {
                    schedules_[intersection.id()].add_street(street.id(), 1);
                }
            }
        }
    }

    void Simulation::add_car_event(Car &car, size_t time) {
        car.move_to_next_street();
        auto street_length = streets_[car.current_street()].length();
        car_events_.emplace(time + street_length, car);
    }

    void Simulation::add_street_event(Street &street, size_t time) {
        auto &&intersection = intersections_[street.end()];
        auto &&next_green_time = schedules_.at(intersection.id()).next_green(street.id(), time);
        if (next_green_time.has_value()) {
            street_events_.emplace(next_green_time.value(), street);
        }
    }

    void Simulation::initialize_run() {
        for (auto &&car: cars_) {
            auto &&street = streets_[car.current_street()];
            street.add_car(car.id());
            if (street.car_queue_size() == 1) {
                add_street_event(street, 0);
            }
        }
    }

    void Simulation::process_car_events() {
        auto current_time = car_events_.top().time();
        while (!car_events_.empty() && current_time == car_events_.top().time()) {
            auto &&car = car_events_.top().car();
            car_events_.pop();

            if (car.final_destination()) {
                car.set_finish_time(current_time);
                continue;
            }

            auto &&street = streets_[car.current_street()];
            street.add_car(car.id());
            if (street.car_queue_size() == 1) {
                add_street_event(street, current_time);
            }
        }
    }

    void Simulation::process_street_events() {
        auto current_time = street_events_.top().time();
        while (!street_events_.empty() && current_time == street_events_.top().time()) {
            auto &&street = street_events_.top().street();
            auto &&car = cars_[street.get_car()];
            street_events_.pop();

            add_car_event(car, current_time);

            if (street.car_queue_size() > 0) {
                // The next green is at (current time + 1) or later
                add_street_event(street, current_time + 1);
            }
        }
    }

    Simulation &Simulation::run() {
        reset_run();
        //TODO: finish refactoring the methods below
        initialize_run();

        while (!car_events_.empty() || !street_events_.empty()) {
            auto car_event_time = car_events_.empty() ? std::numeric_limits<size_t>::max() : car_events_.top().time();

            auto street_event_time = street_events_.empty() ? std::numeric_limits<size_t>::max() : street_events_.top().time();

            if (car_event_time > city_plan_.duration() &&
                street_event_time > city_plan_.duration()) {
                break;
            }

            if (car_event_time <= street_event_time) {
                process_car_events();
            }
            if (car_event_time >= street_event_time) {
                process_street_events();
            }
        }
        return *this;
    }

    size_t Simulation::score() const {
        auto count_score = [this](size_t total, const auto &car) {
            if (car.finished()) {
                total += this->city_plan_.bonus() + this->city_plan_.duration() - car.finish_time();
            }
            return total;
        };
        return std::accumulate(cars_.cbegin(), cars_.cend(), size_t{0}, count_score);
    }

    void Simulation::summary() const {
        size_t finished_count = 0;
        size_t total_ride_time = 0;
        size_t total_score = 0;
        size_t max_score = 0;
        size_t min_score = std::numeric_limits<size_t>::max();
        const Car *earliest_car = nullptr;
        const Car *latest_car = nullptr;
        for (auto &&c: cars_) {
            if (c.finished()) {
                ++finished_count;
                total_ride_time += c.finish_time();
                auto score = city_plan_.bonus() + city_plan_.duration() - c.finish_time();
                total_score += score;
                if (score > max_score) {
                    max_score = score;
                    earliest_car = &c;
                }
                if (score < min_score) {
                    min_score = score;
                    latest_car = &c;
                }
            }
        }
        float average_ride_time = static_cast<float>(total_ride_time) / static_cast<float>(finished_count);

        // return int as a thousand seperated string
        auto thousand_sep = [](size_t i) {
            std::string s = std::to_string(i);
            std::string formatted;

            int count = 0;
            for (int pos = static_cast<int>(s.length()) - 1; pos >= 0; --pos) {
                formatted.insert(0, 1, s[static_cast<size_t>(pos)]);
                ++count;
                if (count % 3 == 0 && pos > 0) {
                    formatted.insert(0, 1, ',');
                }
            }
            return formatted;
        };

        std::cout << "The submission scored **" << thousand_sep(total_score) << " points**. "
                  << "This is the sum of " << thousand_sep(finished_count * city_plan_.bonus()) << " bonus points for "
                  << "cars arriving before the deadline (" << thousand_sep(city_plan_.bonus()) << " points each) and "
                  << thousand_sep(total_score - finished_count * city_plan_.bonus()) << " points for early arrival times.\n\n"
                  << thousand_sep(finished_count) << " of " << thousand_sep(cars_.size()) << " cars arrived before the deadline "
                  << "(" << static_cast<float>(finished_count) / static_cast<float>(cars_.size()) * 100 << "%). "
                  << "The earliest car (ID " << earliest_car->id() << ") arrived at its destination after "
                  << thousand_sep(earliest_car->finish_time()) << " seconds scoring " << thousand_sep(max_score) << " points, "
                  << "whereas the last car (ID " << latest_car->id() << ") arrived at its destination "
                  << "after " << thousand_sep(latest_car->finish_time()) << " seconds scoring " << thousand_sep(min_score) << " points. "
                  << "Cars that arrived within the deadline drove for an average of " << std::fixed
                  << std::setprecision(2) << average_ride_time << " seconds to arrive at their destination.\n";
    }

}
