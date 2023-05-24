#ifndef STREET_SHARED_HPP
#define STREET_SHARED_HPP

#include <string>
#include <utility>

#include "intersection_shared.hpp"

class IntersectionShared;

class StreetShared {
public:
    StreetShared(
        const int id,
        IntersectionShared *const start,
        IntersectionShared *const end,
        std::string name,
        const int length
    ) : id_(id), start_(start), end_(end), name_(std::move(name)), length_(length), used_(false) {}

    int id() const {
        return id_;
    }

    IntersectionShared *const start() {
        return start_;
    }

    IntersectionShared *const end() {
        return end_;
    }

    const std::string &name() const {
        return name_;
    }

    int length() const {
        return length_;
    }

    bool is_used() const {
        return used_;
    }

    void set_used(bool used) {
        used_ = used;
    }

private:
    const int id_;
    IntersectionShared *const start_;
    IntersectionShared *const end_;
    const std::string name_;
    const int length_;
    bool used_;
};


#endif
