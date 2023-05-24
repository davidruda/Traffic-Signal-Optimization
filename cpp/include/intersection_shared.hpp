#ifndef INTERSECTION_SHARED_HPP
#define INTERSECTION_SHARED_HPP

#include <vector>

#include "street_shared.hpp"

class StreetShared;

class IntersectionShared {
public:
    explicit IntersectionShared(const int id) : id_(id) {}

    const int id() const {
        return id_;
    }
    std::vector<const StreetShared *> &incoming() {
        return incoming_;
    }
    std::vector<const StreetShared *> &outgoing() {
        return outgoing_;
    }

private:
    const int id_;
    std::vector<const StreetShared *> incoming_;
    std::vector<const StreetShared *> outgoing_;// redundant
};


#endif
