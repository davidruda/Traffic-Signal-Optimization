#ifndef INTERSECTION_INSTANCE_HPP
#define INTERSECTION_INSTANCE_HPP

#include "intersection_shared.hpp"

class IntersectionInstance {
public:
    explicit IntersectionInstance(const IntersectionShared &data);

private:
    const IntersectionShared &data_;
};


#endif
