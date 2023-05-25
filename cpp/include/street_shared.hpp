#ifndef STREET_SHARED_HPP
#define STREET_SHARED_HPP

#include <string>
#include <utility>

#include "intersection_shared.hpp"

class IntersectionShared;

class StreetShared {
public:
    StreetShared(
            int id,
            IntersectionShared &start,
            IntersectionShared &end,
            std::string name,
            int length);

    int id() const;
    IntersectionShared &start() const;
    IntersectionShared &end() const;
    const std::string &name() const;
    int length() const;
    bool is_used() const;
    void set_used(bool used);

    friend std::ostream &operator<<(std::ostream &os, const StreetShared &obj);

private:
    const int id_;
    IntersectionShared &start_;
    IntersectionShared &end_;
    const std::string name_;
    const int length_;
    bool used_;
};

#endif
