#include <iostream>

#include "city_plan_street.hpp"

namespace city_plan {

    Street::Street(size_t id, size_t start_id, size_t end_id, std::string name, size_t length)
        : id_(id), start_id_(start_id), end_id_(end_id), name_(std::move(name)), length_(length), used_(false) {}

    size_t Street::id() const {
        return id_;
    }

    size_t Street::end() const {
        return end_id_;
    }

    const std::string &Street::name() const {
        return name_;
    }

    void Street::set_used(bool used) {
        used_ = used;
    }

    bool Street::is_used() const {
        return used_;
    }

    size_t Street::length() const {
        return length_;
    }

    std::ostream &operator<<(std::ostream &os, const Street &obj) {
        os << "[" << obj.id_ << " " << obj.name_ << " " << obj.start_id_
           << " -> " << obj.end_id_ << " length: " << obj.length_ << "]";
        return os;
    }

}
