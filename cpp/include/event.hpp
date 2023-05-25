#ifndef EVENT_HPP
#define EVENT_HPP

#include "car_instance.hpp"
#include "street_instance.hpp"

class Event {
public:
    virtual ~Event();

    int time() const;

    bool operator<(const Event &other) const;

protected:
    explicit Event(int time);

    static size_t counter_;
    size_t counter_id_;
    int time_;
};

class CarEvent : public Event {
public:
    CarEvent(int time, CarInstance &car);

private:
    CarInstance &car_;
};

class StreetEvent : public Event {
public:
    StreetEvent(int time, StreetInstance &street);

private:
    StreetInstance &street_;
};

#endif
