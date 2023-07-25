#ifndef EVENT_HPP
#define EVENT_HPP

#include "simulation_car.hpp"
#include "simulation_street.hpp"

namespace simulation {

    class Event {
    public:
        virtual ~Event() = default;

        size_t time() const {
            return time_;
        }

        bool operator<(const Event &other) const;

        enum Type {
            CAR_EVENT_TYPE,
            STREET_EVENT_TYPE
        };

        virtual Type event_type() const = 0;

    protected:
        explicit Event(size_t time);

        static size_t counter_;
        size_t counter_id_;
        size_t time_;
    };

    class CarEvent : public Event {
    public:
        CarEvent(size_t time, Car &car)
            : Event(time), car_(car) {}

        Type event_type() const override {
            return Event::CAR_EVENT_TYPE;
        }

        Car &car() const {
            return car_;
        }

    private:
        Car &car_;
    };

    class StreetEvent : public Event {
    public:
        StreetEvent(size_t time, Street &street)
            : Event(time), street_(street) {}

        Type event_type() const override {
            return Event::STREET_EVENT_TYPE;
        }

        Street &street() const {
            return street_;
        }

    private:
        Street &street_;
    };

}


#endif
