#ifndef INTERSECTION_SHARED_HPP
#define INTERSECTION_SHARED_HPP

#include <functional>
#include <vector>

#include "street_shared.hpp"

class StreetShared;

class IntersectionShared {
public:
    explicit IntersectionShared(int id);

    int id() const;

    void add_incoming(const StreetShared &street);

    void add_outgoing(const StreetShared &street);

    friend std::ostream &operator<<(std::ostream &os, const IntersectionShared &obj);

private:
    const int id_;
    std::vector<std::reference_wrapper<const StreetShared>> incoming_;
    std::vector<std::reference_wrapper<const StreetShared>> outgoing_;// redundant
};

#endif
