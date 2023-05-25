
#include <limits>
#include <stdexcept>

#include "event.hpp"

size_t Event::counter_ = 0;

Event::~Event() = default;

Event::Event(int time) : time_(time) {
    if (counter_ == std::numeric_limits<size_t>::max()) {

        // Doesn't work with Clang in CLion (can't enable exceptions)
        //throw std::overflow_error{"Event counter overflow"};
    }
    counter_id_ = ++counter_;
}

int Event::time() const {
    return time_;
}

bool Event::operator<(const Event &other) const {
    if (time_ == other.time_) {
        return counter_id_ < other.counter_id_;
    }
    return time_ < other.time_;
}

CarEvent::CarEvent(int time, CarInstance &car) : Event(time), car_(car) {}

StreetEvent::StreetEvent(int time, StreetInstance &street) : Event(time), street_(street) {}
