
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <queue>
#include <vector>

#include "event.hpp"
#include "simulation_instance.hpp"

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
        auto count_score = [this](int total, const CarInstance &car) {
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
    const CarInstance *earliest_car = nullptr;
    const CarInstance *latest_car = nullptr;
    for (auto &&c: cars_) {
        if (c.finished()) {
            ++finished_count;
            total_ride_time += c.finish_time();
            int score = data_.bonus() + data_.duration() - c.finish_time();
            ;
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
              << "The earliest car (ID " << earliest_car->data().id() << ") arrived at its destination after "
              << earliest_car->finish_time() << " seconds scoring " << max_score << " points,\n"
              << "whereas the last car (ID " << latest_car->data().id() << ") arrived at its destination "
              << "after " << latest_car->finish_time() << " seconds scoring " << min_score << " points.\n"
              << "Cars that arrived within the deadline drove for an average of "
              << std::setprecision(3) << average_ride_time << " seconds to arrive at their destination.\n";
    return total_score;
}
