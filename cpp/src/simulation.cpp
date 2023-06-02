#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#include "event.hpp"
#include "simulation.hpp"

SimulationShared::SimulationShared(const std::string &filename) {
    std::ifstream data_file{filename};
    int duration;
    int number_of_intersections;
    int number_of_streets;
    int number_of_cars;
    int bonus;

    data_file >> duration >> number_of_intersections >> number_of_streets >> number_of_cars >> bonus;

    duration_ = duration;
    bonus_ = bonus;

    std::cout << duration_ << " " << number_of_intersections << " "
              << number_of_streets << " " << number_of_cars << " "
              << bonus_ << "\n";

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
        std::cout << start << " " << end << " " << name << " " << length << "\n";
        auto &&street = streets_.emplace_back(i, intersections_[start], intersections_[end], name, length);
        street_mapping_.emplace(street.name(), std::ref(street));
        street.start().add_outgoing(street);
        street.end().add_incoming(street);
    }


    int path_length;
    std::string street_name;

    for (int i = 0; i < number_of_cars; ++i) {
        data_file >> path_length;
        std::cout << path_length << " ";
        std::vector<std::reference_wrapper<const Street>> path;
        for (int j = 0; j < path_length; ++j) {
            data_file >> street_name;
            std::cout << street_name << " ";
            auto &&street = street_mapping_.at(street_name).get();
            path.emplace_back(std::ref(street));

            // The last street in path is not marked as used because it doesn't
            // use the traffic light
            if (j < path_length - 1) {
                street.set_used(true);
            }
        }
        cars_.emplace_back(i, path_length, path);
        std::cout << "\n";
    }
}

const std::vector<Intersection> &SimulationShared::intersections() const {
    return intersections_;
}

const std::vector<Street> &SimulationShared::streets() const {
    return streets_;
}

const std::vector<Car> &SimulationShared::cars() const {
    return cars_;
}

std::ostream &operator<<(std::ostream &os, const SimulationShared &obj) {
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

int SimulationShared::duration() const {
    return duration_;
}

int SimulationShared::bonus() const {
    return bonus_;
}

SimulationInstance::SimulationInstance(const SimulationShared &data) : data_(data) {
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

void SimulationInstance::run() {
    int time = 0;

    std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, std::greater<>> event_queue;
    for (auto &&car: cars_) {
        auto &&street = streets_[car.current_street().id()];
        street.add_car(car);
        if (street.car_queue_size() == 1) {
            //TODO: not the right time, the right time is street.next_green
            event_queue.emplace(std::make_unique<StreetEvent>(time, street));
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

            event_queue.emplace(std::make_unique<CarEvent>(time + car.current_street().length(), car));
            if (street.car_queue_size() > 0) {
                //TODO: not the right time, the right time is street.next_green
                event_queue.emplace(std::make_unique<StreetEvent>(time, street));
            }
        } else if (event->event_type() == Event::CAR_EVENT_TYPE) {
            auto &&car = static_cast<CarEvent *>(event.get())->car();
            if (car.at_final_destination()) {
                car.set_finished(true);
                car.set_finish_time(time);
            } else {
                auto &&street = streets_[car.current_street().id()];
                street.add_car(car);
                if (street.car_queue_size() == 1) {
                    //TODO: not the right time, the right time is street.next_green
                    event_queue.emplace(std::make_unique<StreetEvent>(time, street));
                }
            }
        }
    }
}

int SimulationInstance::score(bool verbose) const {
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
    std::cout << "The submission scored ** " << total_score << "  points**.\n"
              << "This is the sum of " << finished_count * data_.bonus() << " bonus points for "
              << "cars arriving before the deadline (" << data_.bonus() << " points each) and "
              << total_score - finished_count * data_.bonus() << " points for early arrival times.\n"
              << finished_count << " of " << cars_.size() << " cars arrived before the deadline "
              << "(" << static_cast<float>(finished_count) / static_cast<float>(cars_.size()) * 100 << "%).\n"
              << "The earliest car (ID " << earliest_car->id() << ") arrived at its destination after "
              << earliest_car->finish_time() << " seconds scoring " << max_score << " points,\n"
              << "whereas the last car (ID " << latest_car->id() << ") arrived at its destination "
              << "after " << latest_car->finish_time() << " seconds scoring " << min_score << " points.\n"
              << "Cars that arrived within the deadline drove for an average of "
              << std::setprecision(3) << average_ride_time << " seconds to arrive at their destination.\n";
    return total_score;
}

const std::vector<Intersection::Instance> &SimulationInstance::intersections() const {
    return intersections_;
}

const std::vector<Street::Instance> &SimulationInstance::streets() const {
    return streets_;
}

const std::vector<Car::Instance> &SimulationInstance::cars() const {
    return cars_;
}
