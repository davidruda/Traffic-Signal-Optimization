#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#include "simulation.hpp"

Simulation::Simulation(const std::string &filename) {
    std::ifstream data_file{filename};
    size_t duration;
    size_t number_of_intersections;
    size_t number_of_streets;
    size_t number_of_cars;
    size_t bonus;

    data_file >> duration >> number_of_intersections >> number_of_streets >> number_of_cars >> bonus;

    duration_ = duration;
    bonus_ = bonus;

#ifdef DEBUG
    std::cout << duration_ << " " << number_of_intersections << " "
              << number_of_streets << " " << number_of_cars << " "
              << bonus_ << "\n";
#endif

    intersections_.reserve(number_of_intersections);
    for (size_t id = 0; id < number_of_intersections; ++id) {
        intersections_.emplace_back(id);
    }

    streets_.reserve(number_of_streets);
    cars_.reserve(number_of_cars);

    size_t start;
    size_t end;
    std::string name;
    size_t length;
    for (size_t id = 0; id < number_of_streets; ++id) {
        data_file >> start >> end >> name >> length;
#ifdef DEBUG
        std::cout << start << " " << end << " " << name << " " << length << "\n";
#endif
        auto &&street = streets_.emplace_back(id, start, end, name, length);
        street_mapping_.emplace(street.name(), id);
        intersections_[end].add_incoming(id);
    }


    size_t path_length;
    std::string street_name;

    for (size_t id = 0; id < number_of_cars; ++id) {
        data_file >> path_length;
#ifdef DEBUG
        std::cout << path_length << " ";
#endif
        std::vector<size_t> path;
        for (size_t i = 0; i < path_length; ++i) {
            data_file >> street_name;
#ifdef DEBUG
            std::cout << street_name << " ";
#endif
            auto &&street_id = street_mapping_[street_name];
            path.emplace_back(street_id);

            // The last street in path is not marked as used because it doesn't
            // use the traffic light
            if (i < path_length - 1) {
                streets_[street_id].set_used(true);
            }
        }
        cars_.emplace_back(id, path_length, path);
#ifdef DEBUG
        std::cout << "\n";
#endif
    }
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

std::ostream &operator<<(std::ostream &os, const Simulation &obj) {
    os << "Duration: " << obj.duration_ << "\n"
       << "Number of intersections: " << obj.intersections_.size() << "\n"
       << "Bonus: " << obj.bonus_ << "\n"
       << "Number of streets: " << obj.streets_.size() << "\n";
    for (auto &&s: obj.streets_) {
        os << s << "\n";
    }
    os << "Number of cars: " << obj.cars_.size() << "\n";
    for (auto &&c: obj.cars_) {
        os << c << "\n";
    }
    return os;
}

size_t Simulation::duration() const {
    return duration_;
}

size_t Simulation::bonus() const {
    return bonus_;
}

Simulation::Instance Simulation::create_instance() {
    return Simulation::Instance{*this};
}

Simulation::Instance::Instance(const Simulation &data) : data_(data) {
    intersections_.reserve(data.intersections().size());
    streets_.reserve(data.streets().size());
    cars_.reserve(data.cars().size());

    for (auto &&i: data.intersections()) {
        intersections_.emplace_back(i);
    }
    for (auto &&s: data.streets()) {
        streets_.emplace_back(s);
    }
    for (auto &&c: data.cars()) {
        cars_.emplace_back(c);
    }
}

void Simulation::Instance::read_plan(const std::string &filename) {
    std::ifstream file{filename};
    size_t number_of_intersections;
    size_t intersection_id;
    size_t number_of_streets;
    std::string street_name;
    size_t green_light_duration;

    file >> number_of_intersections;
    for (size_t i = 0; i < number_of_intersections; ++i) {
        file >> intersection_id;
        schedules_.emplace(intersection_id, intersection_id);

        file >> number_of_streets;
        for (size_t j = 0; j < number_of_streets; ++j) {
            file >> street_name >> green_light_duration;
            auto &&street_id = data_.street_mapping_.at(street_name);
            schedules_.at(intersection_id).add_street(street_id, green_light_duration);
        }
    }
}

void Simulation::Instance::write_plan(const std::string &filename) {
    std::ofstream file{filename};
    file << schedules_.size() << "\n";

    for (auto &&[intersection_id, schedule]: schedules_) {
        file << intersection_id << "\n"
             << schedule.length() << "\n";
        //TODO: finish this
    }
    //for (const Intersection::Instance &s: scheduled) {
    //    auto &&schedule = s.schedule().value().get();
    //    file << s.id() << "\n"
    //         << schedule.length() << "\n";
    //
    //    std::vector<std::pair<size_t, std::ranges::iota_view<size_t, size_t>>> schedule_map{schedule.cbegin(), schedule.cend()};
    //    auto cmp = [](const auto &lhs, const auto &rhs) {
    //        return *lhs.second.begin() < *rhs.second.begin();
    //    };
    //    std::sort(schedule_map.begin(), schedule_map.end(), cmp);
    //
    //    for (auto &&[street_id, range]: schedule_map) {
    //        auto &&street_name = streets_[street_id].name();
    //        auto green_light_duration = (*range.end() - *range.begin());
    //        file << street_name << " " << green_light_duration << "\n";
    //    }
    //}
}

void Simulation::Instance::create_plan_default() {
    for (auto &&intersection: intersections_) {
        schedules_.emplace(intersection.id(), intersection.id());
        for (auto &&s: intersection.incoming()) {
            auto &&street = streets_[s];
            if (street.is_used()) {
                schedules_.at(intersection.id()).add_street(street.id(), 1);
            }
        }
    }
}

void Simulation::Instance::initialize_event_queue(auto &event_queue) {
    for (auto &&car: cars_) {
        auto &&street = streets_[car.current_street()];
        street.add_car(car.id());
        if (street.car_queue_size() == 1) {

            auto &&intersection = intersections_[street.end()];
            auto &&next_green_time = schedules_.at(intersection.id()).next_green(street, 0);
            if (next_green_time.has_value()) {
                event_queue.emplace(std::make_unique<StreetEvent>(next_green_time.value(), street));
            }
        }
    }
}

void Simulation::Instance::process_street_event(auto &event_queue, auto &event) {
    auto &&street_event = static_cast<StreetEvent *>(event.get());
    auto time = street_event->time();
    auto &&street = street_event->street();
    auto &&car = cars_[street.get_car(time)];
    car.move_to_next_street();

    event_queue.pop();
    event_queue.emplace(std::make_unique<CarEvent>(time + streets_[car.current_street()].length(), car));

    if (street.car_queue_size() > 0) {

        auto &&intersection = intersections_[street.end()];
        auto &&next_green_time = schedules_.at(intersection.id()).next_green(street, time);
        if (next_green_time.has_value()) {
            event_queue.emplace(std::make_unique<StreetEvent>(next_green_time.value(), street));
        }
    }
}

void Simulation::Instance::process_car_event(auto &event_queue, auto &event) {
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
            auto &&next_green_time = schedules_.at(intersection.id()).next_green(street, time);
            if (next_green_time.has_value()) {
                event_queue.emplace(std::make_unique<StreetEvent>(next_green_time.value(), street));
            }
        }
    }
}

Simulation::Instance &Simulation::Instance::run() {
    // an earlier event has higher priority
    auto cmp = [](const std::unique_ptr<Event> &lhs, const std::unique_ptr<Event> &rhs) {
        return !(*lhs < *rhs);
    };
    std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, decltype(cmp)> event_queue;
    //TODO: finish refactoring the methods below
    initialize_event_queue(event_queue);

    while (!event_queue.empty()) {
        auto &&event = event_queue.top();
        if (event->time() > data_.duration_) {
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

size_t Simulation::Instance::score(bool verbose) const {
    if (!verbose) {
        auto count_score = [this](size_t total, const Car::Instance &car) {
            if (car.finished()) {
                total += this->data_.bonus_ + this->data_.duration_ - car.finish_time();
            }
            return total;
        };
        return std::accumulate(cars_.cbegin(), cars_.cend(), 0, count_score);
    }
    size_t finished_count = 0;
    size_t total_ride_time = 0;
    size_t total_score = 0;
    size_t max_score = 0;
    size_t min_score = std::numeric_limits<size_t>::max();
    const Car::Instance *earliest_car = nullptr;
    const Car::Instance *latest_car = nullptr;
    for (auto &&c: cars_) {
        if (c.finished()) {
            ++finished_count;
            total_ride_time += c.finish_time();
            auto score = data_.bonus_ + data_.duration_ - c.finish_time();
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
              << "This is the sum of " << thousand_sep(finished_count * data_.bonus_) << " bonus points for "
              << "cars arriving before the deadline (" << thousand_sep(data_.bonus_) << " points each) and "
              << thousand_sep(total_score - finished_count * data_.bonus_) << " points for early arrival times.\n\n"
              << thousand_sep(finished_count) << " of " << thousand_sep(cars_.size()) << " cars arrived before the deadline "
              << "(" << static_cast<float>(finished_count) / static_cast<float>(cars_.size()) * 100 << "%). "
              << "The earliest car (ID " << earliest_car->id() << ") arrived at its destination after "
              << thousand_sep(earliest_car->finish_time()) << " seconds scoring " << thousand_sep(max_score) << " points, "
              << "whereas the last car (ID " << latest_car->id() << ") arrived at its destination "
              << "after " << thousand_sep(latest_car->finish_time()) << " seconds scoring " << thousand_sep(min_score) << " points. "
              << "Cars that arrived within the deadline drove for an average of " << std::fixed
              << std::setprecision(2) << average_ride_time << " seconds to arrive at their destination.\n";
    return total_score;
}

const std::vector<Intersection::Instance> &Simulation::Instance::intersections() const {
    return intersections_;
}

const std::vector<Street::Instance> &Simulation::Instance::streets() const {
    return streets_;
}

const std::vector<Car::Instance> &Simulation::Instance::cars() const {
    return cars_;
}
