
#include <iostream>

#include "intersection.hpp"
#include "street.hpp"

Intersection::Intersection(size_t id) : id_(id) {}

size_t Intersection::id() const {
    return id_;
}

void Intersection::add_incoming(const Street &street) {
    incoming_.emplace_back(std::cref(street));
}

//const std::vector<std::reference_wrapper<const Street>> &Intersection::incoming() const {
//    return incoming_;
//}

std::ostream &operator<<(std::ostream &os, const Intersection &obj) {
    os << obj.id_ << "\n"
       << "Incoming streets:\n";
    for (auto &&s: obj.incoming_) {
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

void Intersection::Instance::add_street_to_schedule(size_t street_id, size_t green_light_duration) {
    schedule_.add_street(street_id, green_light_duration);
}

std::optional<size_t> Intersection::Instance::next_green(size_t time, const Street::Instance &street) {
    return schedule_.next_green(time, street);
}

size_t Intersection::Instance::id() const {
    return data_.id_;
}

const std::vector<std::reference_wrapper<const Street>> &Intersection::Instance::incoming() const {
    return data_.incoming_;
}
