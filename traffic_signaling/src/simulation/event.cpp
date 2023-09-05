#include <cassert>
#include <limits>

#include "simulation/event.hpp"

namespace simulation {

size_t Event::counter_{};

Event::Event(size_t time)
    : time_(time) {
    assert(counter_ < std::numeric_limits<size_t>::max());
    counter_id_ = counter_++;
}

bool Event::operator<(const Event &other) const {
    if (time_ == other.time_) {
        return counter_id_ < other.counter_id_;
    }
    return time_ < other.time_;
}

bool Event::operator>(const Event &other) const {
    if (time_ == other.time_) {
        return counter_id_ > other.counter_id_;
    }
    return time_ > other.time_;
}
}
