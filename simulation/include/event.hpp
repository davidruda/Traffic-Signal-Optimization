#ifndef EVENT_HPP
#define EVENT_HPP

#include "car.hpp"
#include "street.hpp"

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
    CarEvent(size_t time, Car::Instance &car);

    Type event_type() const override;
    Car::Instance &car() const;

private:
    Car::Instance &car_;
};

class StreetEvent : public Event {
public:
    StreetEvent(size_t time, Street::Instance &street);

    Type event_type() const override;
    Street::Instance &street() const;

private:
    Street::Instance &street_;
};

#endif
