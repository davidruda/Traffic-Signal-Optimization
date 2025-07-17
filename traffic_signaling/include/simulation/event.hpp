#ifndef SIMULATION_EVENT_HPP
#define SIMULATION_EVENT_HPP

#include <functional>

#include "simulation/street.hpp"

namespace simulation {
/**
 * Event in the simulation.
 *
 * Serves as an abstract base class for events in the event queue.
 */
class Event {
public:
    virtual ~Event() = default;

    /**
     * Return the time the event occurs.
     */
    unsigned long time() const {
        return time_;
    }

    /**
     * Return True if this event occurs before the other event.
     */
    bool operator<(const Event &other) const;
    /**
     * Return True if this event occurs after the other event.
     */
    bool operator>(const Event &other) const;

    /** Type of the event. */
    enum class Type {
        STREET_EVENT_TYPE
    };

    /**
     * Return the type of the event.
     *
     * Used to determine the type of the event when processing it in the event queue.
     */
    virtual Type event_type() const = 0;

protected:
    /**
     * Construct an event occurring at the given time.
     *
     * @param time Time of the event occurrence.
     */
    explicit Event(unsigned long time);

    /**
     * Counter for unique event IDs.
     *
     * Used to break ties in the event queue when two events occur at the same time.
     */
    static size_t counter_;
    /** Unique ID of the event. */
    size_t counter_id_;
    /** Time of the event occurrence. */
    unsigned long time_;
};

/**
 * Street event in the simulation.
 *
 * Represents the moment when a car on this street has the green light and moves to the next street.
 */
class StreetEvent : public Event {
public:
    /**
     * Construct a street event with a given time and street.
     *
     * @param time Time of the event in occurrence.
     * @param street Street the event is associated with.
     */
    StreetEvent(unsigned long time, Street &street)
        : Event(time), street_(street) {}

    /**
     * Return the type of the event.
     */
    Type event_type() const override {
        return Type::STREET_EVENT_TYPE;
    }

    /**
     * Return the street associated with this event.
     */
    Street &street() const {
        return street_;
    }

private:
    /** Street of this event. */
    std::reference_wrapper<Street> street_;
};
}

#endif
