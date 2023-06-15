#ifndef CITY_PLAN_INTERSECTION_HPP
#define CITY_PLAN_INTERSECTION_HPP

#include <vector>

class CityPlanIntersection {
public:
    explicit CityPlanIntersection(size_t id);

    void add_incoming(size_t street_id);
    size_t id() const;
    const std::vector<size_t> &incoming() const;

    friend std::ostream &operator<<(std::ostream &os, const CityPlanIntersection &obj);

private:
    const size_t id_;
    // incoming streets represented by street ids
    std::vector<size_t> incoming_streets_;
};


#endif
