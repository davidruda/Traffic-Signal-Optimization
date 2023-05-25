
#include <queue>
#include <vector>
#include <functional>

#include "simulation_instance.hpp"
#include "event.hpp"

SimulationInstance::SimulationInstance(const SimulationShared &data) : data_(data) {
    intersections_.reserve(data.intersections().size());
    streets_.reserve(data.streets().size());
    cars_.reserve(data.cars().size());

    for (auto&& i:data.intersections()) {
        intersections_.emplace_back(i);
    }
    for (auto&& s:data.streets()) {
        streets_.emplace_back(s);
    }
    for (auto&& c:data.cars()) {
        cars_.emplace_back(c);
    }
}

void SimulationInstance::run() {
    int time = 0;

    std::priority_queue<Event, std::vector<Event>, std::greater<>> event_queue;
    for (auto&& car:cars_) {
        auto&& street = streets_[car.current_street_index()];
        street.add_car(car);
    }
}
