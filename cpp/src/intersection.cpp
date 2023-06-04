
#include <iostream>

#include "intersection.hpp"
#include "street.hpp"

Intersection::Intersection(int id) : id_(id) {}

int Intersection::id() const {
    return id_;
}

void Intersection::add_incoming(const Street &street) {
    incoming_.emplace_back(std::cref(street));
}

void Intersection::add_outgoing(const Street &street) {
    outgoing_.emplace_back(std::cref(street));
}

const std::vector<std::reference_wrapper<const Street>> &Intersection::incoming() const {
    return incoming_;
}

std::ostream &operator<<(std::ostream &os, const Intersection &obj) {
    os << obj.id_ << "\n"
       << "Incoming streets:\n";
    for (auto &&s: obj.incoming_) {
        os << s.get() << "\n";
    }
    os << "Outgoing streets:\n";
    for (auto &&s: obj.outgoing_) {
        os << s.get() << "\n";
    }
    return os;
}

Intersection::Instance::Instance(const Intersection &data)
    : data_(data), schedule_(data_.id_) {}

bool Intersection::Instance::has_schedule() const {
    return schedule_.length() > 0;
}

void Intersection::Instance::reset_schedule() {
    schedule_ = Schedule{data_.id_};
}

std::optional<std::reference_wrapper<const Schedule>> Intersection::Instance::schedule() const {
    if (schedule_.length() > 0) {
        return std::cref(schedule_);
    }
    return {};
}

void Intersection::Instance::add_street_to_schedule(int street_id, int green_light_duration) {
    schedule_.add_street(street_id, green_light_duration);
}

std::optional<int> Intersection::Instance::next_green(int time, const Street::Instance &street) {
    return schedule_.next_green(time, street);
}

int Intersection::Instance::id() const {
    return data_.id_;
}

const std::vector<std::reference_wrapper<const Street>> &Intersection::Instance::incoming() const {
    return data_.incoming_;
}
