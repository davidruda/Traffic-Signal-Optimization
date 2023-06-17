#include <algorithm>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#include "event.hpp"
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

                // Sort by the order in which the green light appears in the schedule
                auto &&green_lights = schedule.green_lights();
                std::vector<std::pair<size_t, std::ranges::iota_view<size_t, size_t>>> schedule_sorted{green_lights.begin(), green_lights.end()};
                auto cmp = [](const auto &lhs, const auto &rhs) {
                    return *lhs.second.begin() < *rhs.second.begin();
                };
                std::sort(schedule_sorted.begin(), schedule_sorted.end(), cmp);

                for (auto &&[street_id, green_light]: schedule_sorted) {
                    auto green_light_duration = *green_light.end() - *green_light.begin();
                    file << streets_[street_id].name() << " " << green_light_duration << "\n";
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

    void Simulation::initialize_event_queue(auto &event_queue) {
        for (auto &&car: cars_) {
            auto &&street = streets_[car.current_street()];
            street.add_car(car.id());
            if (street.car_queue_size() == 1) {

                auto &&intersection = intersections_[street.end()];
                auto &&next_green_time = schedules_.at(intersection.id()).next_green(street.id(), 0, street.last_used_time());
                if (next_green_time.has_value()) {
                    event_queue.emplace(std::make_unique<StreetEvent>(next_green_time.value(), street));
                }
            }
        }
    }

    void Simulation::process_street_event(auto &event_queue, auto &event) {
        auto &&street_event = static_cast<StreetEvent *>(event.get());
        auto time = street_event->time();
        auto &&street = street_event->street();
        auto &&car = cars_[street.get_car(time)];
        car.move_to_next_street();

        event_queue.pop();
        event_queue.emplace(std::make_unique<CarEvent>(time + streets_[car.current_street()].length(), car));

        if (street.car_queue_size() > 0) {

            auto &&intersection = intersections_[street.end()];
            auto &&next_green_time = schedules_.at(intersection.id()).next_green(street.id(), time, street.last_used_time());
            if (next_green_time.has_value()) {
                event_queue.emplace(std::make_unique<StreetEvent>(next_green_time.value(), street));
            }
        }
    }

    void Simulation::process_car_event(auto &event_queue, auto &event) {
        auto &&car_event = static_cast<CarEvent *>(event.get());
        auto time = car_event->time();
        auto &&car = static_cast<CarEvent *>(event.get())->car();
        event_queue.pop();
        if (car.at_final_destination()) {
            car.set_finished(true);
            car.set_finish_time(time);
        } else {
            auto &&street = streets_[car.current_street()];
            street.add_car(car.id());
            if (street.car_queue_size() == 1) {

                auto &&intersection = intersections_[street.end()];
                auto &&next_green_time = schedules_.at(intersection.id()).next_green(street.id(), time, street.last_used_time());
                if (next_green_time.has_value()) {
                    event_queue.emplace(std::make_unique<StreetEvent>(next_green_time.value(), street));
                }
            }
        }
    }

    Simulation &Simulation::run() {
        reset_run();
        // an earlier event has higher priority
        auto cmp = [](const std::unique_ptr<Event> &lhs, const std::unique_ptr<Event> &rhs) {
            return !(*lhs < *rhs);
        };
        std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, decltype(cmp)> event_queue;
        //TODO: finish refactoring the methods below
        initialize_event_queue(event_queue);

        while (!event_queue.empty()) {
            auto &&event = event_queue.top();
            if (event->time() > city_plan_.duration()) {
                break;
            }

            if (event->event_type() == Event::STREET_EVENT_TYPE) {
                process_street_event(event_queue, event);
            } else if (event->event_type() == Event::CAR_EVENT_TYPE) {
                process_car_event(event_queue, event);
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
        return std::accumulate(cars_.cbegin(), cars_.cend(), 0, count_score);
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

    const std::vector<Intersection> &Simulation::intersections() const {
        return intersections_;
    }

    const std::vector<Street> &Simulation::streets() const {
        return streets_;
    }

    const std::vector<Car> &Simulation::cars() const {
        return cars_;
    }

}// namespace simulation
