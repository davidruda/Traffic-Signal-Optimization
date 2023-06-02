#ifndef EVENT_HPP
#define EVENT_HPP

#include "car.hpp"
#include "street.hpp"

class Event {
public:
    virtual ~Event();

    int time() const;

    bool operator<(const Event &other) const;

    enum EventType {
        CAR_EVENT_TYPE,
        STREET_EVENT_TYPE
    };

    virtual EventType event_type() const = 0;

protected:
    explicit Event(int time);

    static size_t counter_;
    size_t counter_id_;
    int time_;
};

class CarEvent : public Event {
public:
    CarEvent(int time, Car::Instance &car);

    EventType event_type() const override;
    Car::Instance &car() const;

private:
    Car::Instance &car_;
};

class StreetEvent : public Event {
public:
    StreetEvent(int time, Street::Instance &street);

    EventType event_type() const override;
    Street::Instance &street() const;

private:
    Street::Instance &street_;
};

#endif
