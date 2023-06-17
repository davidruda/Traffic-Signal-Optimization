#include <limits>
#include <stdexcept>

#include "event.hpp"

namespace simulation {

    size_t Event::counter_ = 0;

    Event::~Event() = default;

    Event::Event(size_t time) : time_(time) {
        if (counter_ == std::numeric_limits<size_t>::max()) {
            // Doesn't work with Clang-cl in CLion (can't enable exceptions)
            //throw std::overflow_error{"Event counter overflow"};
        }
        counter_id_ = counter_++;
    }

    size_t Event::time() const {
        return time_;
    }

    bool Event::operator<(const Event &other) const {
        if (time_ == other.time_) {
            return counter_id_ < other.counter_id_;
        }
        return time_ < other.time_;
    }

    CarEvent::CarEvent(size_t time, Car &car) : Event(time), car_(car) {}

    Event::Type CarEvent::event_type() const {
        return Event::CAR_EVENT_TYPE;
    }

    Car &CarEvent::car() const {
        return car_;
    }

    StreetEvent::StreetEvent(size_t time, Street &street) : Event(time), street_(street) {}

    Event::Type StreetEvent::event_type() const {
        return Event::STREET_EVENT_TYPE;
    }

    Street &StreetEvent::street() const {
        return street_;
    }

}// namespace simulation
