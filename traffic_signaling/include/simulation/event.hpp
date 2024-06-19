#ifndef SIMULATION_EVENT_HPP
#define SIMULATION_EVENT_HPP

#include <functional>

#include "simulation/street.hpp"

namespace simulation {
class Event {
public:
    virtual ~Event() = default;

    unsigned long time() const {
        return time_;
    }

    bool operator<(const Event &other) const;
    bool operator>(const Event &other) const;

    enum class Type {
        STREET_EVENT_TYPE
    };

    virtual Type event_type() const = 0;

protected:
    explicit Event(unsigned long time);

    static size_t counter_;
    size_t counter_id_;
    unsigned long time_;
};

class StreetEvent : public Event {
public:
    StreetEvent(unsigned long time, Street &street)
        : Event(time), street_(street) {}

    Type event_type() const override {
        return Type::STREET_EVENT_TYPE;
    }

    Street &street() const {
        return street_;
    }

private:
    std::reference_wrapper<Street> street_;
};
}

#endif
