#ifndef EVENT_HPP
#define EVENT_HPP

#include "simulation_car.hpp"
#include "simulation_street.hpp"

namespace simulation {

    class Event {
    public:
        virtual ~Event();

        size_t time() const;

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
        CarEvent(size_t time, Car &car);

        Type event_type() const override;
        Car &car() const;

    private:
        Car &car_;
    };

    class StreetEvent : public Event {
    public:
        StreetEvent(size_t time, Street &street);

        Type event_type() const override;
        Street &street() const;

    private:
        Street &street_;
    };

}

#endif
