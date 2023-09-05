#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>

#include "simulation/simulation.hpp"

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

    void Simulation::initialize_run() {
        for (auto &&car: cars_) {
            add_event(car, 0);
        }
    }

    void Simulation::add_event(Car &car, size_t time) {
        auto &&street = streets_[car.current_street()];
        auto &&intersection = intersections_[street.end()];

        auto latest_used_time = street.latest_used_time();
        if (latest_used_time.has_value()) {
            time = (*latest_used_time >= time) ? *latest_used_time + 1 : time;
        }

        auto next_green_time = schedules_.at(intersection.id()).next_green(street.id(), time);
        if (next_green_time.has_value()) {
            street.add_car(car.id(), *next_green_time);
            event_queue_.emplace(*next_green_time, street);
        }
    }

    void Simulation::process_event() {
        auto current_time = event_queue_.top().time();
        auto &&car = cars_[event_queue_.top().street().get_car()];
        event_queue_.pop();

        car.move_to_next_street();
        auto &&next_street = streets_[car.current_street()];

        if (car.final_destination()) {
            auto finish_time = current_time + next_street.length();
            if (finish_time <= city_plan_.duration()) {
                car.set_finish_time(finish_time);
                stats_.update(city_plan_, car);
            }
            return;
        }
        add_event(car, current_time + next_street.length());
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

    size_t Simulation::score(bool verbose) {
        Statistics::reset(*this, verbose);
        run();
        if (verbose) {
            stats_.summary(city_plan_);
        }
        return stats_.total_score_;
    }

    void Simulation::Statistics::reset(Simulation &simulation, bool verbose) {
        if (!verbose) {
            simulation.stats_.total_score_ = {};
            simulation.stats_.verbose_ = verbose;
            return;
        }
        simulation.stats_ = Statistics{verbose};
    }

    void Simulation::Statistics::update(const city_plan::CityPlan &city_plan, const Car &car) {
        auto car_score = city_plan.bonus() + city_plan.duration() - car.finish_time();
        total_score_ += car_score;
        if (!verbose_) {
            return;
        }
        ++cars_finished_;
        total_driving_time_ += car.finish_time();
        if (car_score > max_car_score_) {
            max_car_score_ = car_score;
            earliest_car_ = car;
        }
        if (car_score < min_car_score_) {
            min_car_score_ = car_score;
            latest_car_ = car;
        }
    }

    void Simulation::Statistics::summary(const city_plan::CityPlan &city_plan) {
        // Ensure that the thousand separator is used for printing numbers
        // without relying on other locales, which may not be available.
        std::locale custom_locale{std::locale{}, new ThousandSeparator};
        std::cout.imbue(custom_locale);

        std::cout << "The submission scored **" << total_score_ << " points**. "
                  << "This is the sum of " << cars_finished_ * city_plan.bonus() << " bonus points for "
                  << "cars arriving before the deadline (" << city_plan.bonus() << " points each) and "
                  << total_score_ - cars_finished_ * city_plan.bonus() << " points for early arrival times.\n\n"
                  << cars_finished_ << " of " << city_plan.cars().size() << " cars arrived before the deadline (";
        auto finished_percentage = static_cast<float>(cars_finished_) / static_cast<float>(city_plan.cars().size()) * 100;
        std::cout << std::fixed << std::setprecision(2) << finished_percentage << "%). ";

        if (earliest_car_ && latest_car_) {
            average_drive_time_ = static_cast<float>(total_driving_time_) / static_cast<float>(cars_finished_);

            std::cout << "The earliest car (ID " << earliest_car_->get().id() << ") arrived at its destination after "
                      << earliest_car_->get().finish_time() << " seconds scoring " << max_car_score_
                      << " points, whereas the last car (ID " << latest_car_->get().id() << ") arrived at its destination "
                      << "after " << latest_car_->get().finish_time() << " seconds scoring "
                      << min_car_score_ << " points. Cars that arrived within the deadline drove for an average of "
                      << average_drive_time_ << " seconds to arrive at their destination.";
        }
        std::cout << "\n";
    }
}
