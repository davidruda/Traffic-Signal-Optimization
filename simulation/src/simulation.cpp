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

#include "event.hpp"
#include "simulation.hpp"

Simulation::Simulation(const std::string &filename) {
    std::ifstream data_file{filename};
    int duration;
    int number_of_intersections;
    int number_of_streets;
    int number_of_cars;
    int bonus;

    data_file >> duration >> number_of_intersections >> number_of_streets >> number_of_cars >> bonus;

    duration_ = duration;
    bonus_ = bonus;

#ifdef DEBUG
    std::cout << duration_ << " " << number_of_intersections << " "
              << number_of_streets << " " << number_of_cars << " "
              << bonus_ << "\n";
#endif

    intersections_.reserve(number_of_intersections);
    for (int i = 0; i < number_of_intersections; ++i) {
        intersections_.emplace_back(i);
    }

    streets_.reserve(number_of_streets);
    cars_.reserve(number_of_cars);


    int start;
    int end;
    std::string name;
    int length;
    for (int i = 0; i < number_of_streets; ++i) {
        data_file >> start >> end >> name >> length;
#ifdef DEBUG
        std::cout << start << " " << end << " " << name << " " << length << "\n";
#endif
        auto &&street = streets_.emplace_back(i, intersections_[start], intersections_[end], name, length);
        street_mapping_.emplace(street.name(), std::ref(street));
        street.start().add_outgoing(street);
        street.end().add_incoming(street);
    }


    int path_length;
    std::string street_name;

    for (int i = 0; i < number_of_cars; ++i) {
        data_file >> path_length;
#ifdef DEBUG
        std::cout << path_length << " ";
#endif
        std::vector<std::reference_wrapper<const Street>> path;
        for (int j = 0; j < path_length; ++j) {
            data_file >> street_name;
#ifdef DEBUG
            std::cout << street_name << " ";
#endif
            auto &&street = street_mapping_.at(street_name).get();
            path.emplace_back(std::ref(street));

            // The last street in path is not marked as used because it doesn't
            // use the traffic light
            if (j < path_length - 1) {
                street.set_used(true);
            }
        }
        cars_.emplace_back(i, path_length, path);
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

int Simulation::duration() const {
    return duration_;
}

int Simulation::bonus() const {
    return bonus_;
}

const Street &Simulation::street_by_name(const std::string &name) const {
    return street_mapping_.at(name).get();
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
    int number_of_intersections;
    int intersection_id;
    int number_of_streets;
    std::string street_name;
    int green_light_duration;

    file >> number_of_intersections;
    for (int i = 0; i < number_of_intersections; ++i) {
        file >> intersection_id;
        auto &&intersection = intersections_[intersection_id];

        file >> number_of_streets;
        for (int j = 0; j < number_of_streets; ++j) {
            file >> street_name >> green_light_duration;
            auto &&street_id = data_.street_mapping_.at(street_name).get().id();
            intersection.add_street_to_schedule(street_id, green_light_duration);
        }
    }
}

void Simulation::Instance::write_plan(const std::string &filename) {
    std::ofstream file{filename};
    std::vector<std::reference_wrapper<Intersection::Instance>> scheduled;
    for (auto &&s: intersections_) {
        if (s.has_schedule()) {
            scheduled.emplace_back(std::ref(s));
        }
    }
    file << scheduled.size() << "\n";

    for (const Intersection::Instance &s: scheduled) {
        auto &&schedule = s.schedule().value().get();
        file << s.id() << "\n"
             << schedule.length() << "\n";

        std::vector<std::pair<int, std::ranges::iota_view<int, int>>> schedule_map{schedule.cbegin(), schedule.cend()};
        auto cmp = [](const auto &lhs, const auto &rhs) {
            return *lhs.second.begin() < *rhs.second.begin();
        };
        std::sort(schedule_map.begin(), schedule_map.end(), cmp);

        for (auto &&[street_id, range]: schedule_map) {
            auto &&street_name = streets_[street_id].name();
            auto green_light_duration = (*range.end() - *range.begin());
            file << street_name << " " << green_light_duration << "\n";
        }
    }
}

void Simulation::Instance::create_plan_default() {
    for (auto &&intersection: intersections_) {
        for (auto &&s: intersection.incoming()) {
            auto &&street = s.get();
            if (street.is_used()) {
                intersection.add_street_to_schedule(street.id(), 1);
            }
        }
    }
}

Simulation::Instance &Simulation::Instance::run() {
    int time = 0;

    // an earlier event has higher priority
    auto cmp = [](const std::unique_ptr<Event> &lhs, const std::unique_ptr<Event> &rhs) {
        auto res = !(*lhs < *rhs);
        return res;
        //return lhs.get() >= rhs.get();
    };
    std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, decltype(cmp)> event_queue;
    for (auto &&car: cars_) {
        auto &&street = streets_[car.current_street().id()];
        street.add_car(car);
        if (street.car_queue_size() == 1) {

            auto &&intersection = intersections_[street.end().id()];
            auto &&next_green_time = intersection.next_green(time, street);
            if (next_green_time.has_value()) {
                event_queue.emplace(std::make_unique<StreetEvent>(next_green_time.value(), street));
            }
        }
    }

    while (!event_queue.empty()) {
        auto &&event = event_queue.top();
        time = event->time();
        if (time > data_.duration()) {
            break;
        }

        if (event->event_type() == Event::STREET_EVENT_TYPE) {
            auto &&street = static_cast<StreetEvent *>(event.get())->street();
            auto &&car = street.get_car(time);
            car.move_to_next_street();

            event_queue.pop();
            event_queue.emplace(std::make_unique<CarEvent>(time + car.current_street().length(), car));

            if (street.car_queue_size() > 0) {

                auto &&intersection = intersections_[street.end().id()];
                auto &&next_green_time = intersection.next_green(time, street);
                if (next_green_time.has_value()) {
                    event_queue.emplace(std::make_unique<StreetEvent>(next_green_time.value(), street));
                }
            }
        } else if (event->event_type() == Event::CAR_EVENT_TYPE) {
            auto &&car = static_cast<CarEvent *>(event.get())->car();
            event_queue.pop();
            if (car.at_final_destination()) {
                car.set_finished(true);
                car.set_finish_time(time);
            } else {
                auto &&street = streets_[car.current_street().id()];
                street.add_car(car);
                if (street.car_queue_size() == 1) {

                    auto &&intersection = intersections_[street.end().id()];
                    auto &&next_green_time = intersection.next_green(time, street);
                    if (next_green_time.has_value()) {
                        event_queue.emplace(std::make_unique<StreetEvent>(next_green_time.value(), street));
                    }
                }
            }
        }
    }
    return *this;
}

int Simulation::Instance::score(bool verbose) const {
    if (!verbose) {
        auto count_score = [this](int total, const Car::Instance &car) {
            if (car.finished()) {
                total += this->data_.bonus() + this->data_.duration() - car.finish_time();
            }
            return total;
        };
        return std::accumulate(cars_.cbegin(), cars_.cend(), 0, count_score);
    }
    int finished_count = 0;
    int total_ride_time = 0;
    int total_score = 0;
    int max_score = 0;
    int min_score = std::numeric_limits<int>::max();
    const Car::Instance *earliest_car = nullptr;
    const Car::Instance *latest_car = nullptr;
    for (auto &&c: cars_) {
        if (c.finished()) {
            ++finished_count;
            total_ride_time += c.finish_time();
            int score = data_.bonus() + data_.duration() - c.finish_time();
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

    auto thousand_sep = [](int i) {
        std::string s = std::to_string(i);
        std::string formatted;

        int count = 0;
        for (int pos = static_cast<int>(s.length()) - 1; pos >= 0; --pos) {
            formatted.insert(0, 1, s[pos]);
            ++count;
            if (count % 3 == 0 && pos > 0) {
                formatted.insert(0, 1, ',');
            }
        }
        return formatted;
    };

    std::cout << "The submission scored **" << thousand_sep(total_score) << " points**. "
              << "This is the sum of " << thousand_sep(finished_count * data_.bonus()) << " bonus points for "
              << "cars arriving before the deadline (" << thousand_sep(data_.bonus()) << " points each) and "
              << thousand_sep(total_score - finished_count * data_.bonus()) << " points for early arrival times.\n\n"
              << thousand_sep(finished_count) << " of " << thousand_sep(static_cast<int>(cars_.size())) << " cars arrived before the deadline "
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

const Street &Simulation::Instance::street_by_name(const std::string &name) const {
    return data_.street_mapping_.at(name).get();
}
