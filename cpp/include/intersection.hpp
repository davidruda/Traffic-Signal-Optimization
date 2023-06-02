#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <functional>
#include <vector>

#include "street.hpp"

class StreetShared;

class IntersectionShared {
public:
    explicit IntersectionShared(int id);

    int id() const;

    void add_incoming(const StreetShared &street);

    void add_outgoing(const StreetShared &street);

    const std::vector<std::reference_wrapper<const StreetShared>> &incoming() const;

    friend std::ostream &operator<<(std::ostream &os, const IntersectionShared &obj);

private:
    const int id_;
    std::vector<std::reference_wrapper<const StreetShared>> incoming_;
    //TODO: outgoing is never needed and therefore redundant
    std::vector<std::reference_wrapper<const StreetShared>> outgoing_;
};

class IntersectionInstance {
public:
    explicit IntersectionInstance(const IntersectionShared &data);

    const IntersectionShared &data() const;

private:
    const IntersectionShared &data_;
};

#endif
