#ifndef CITY_PLAN_CAR_HPP
#define CITY_PLAN_CAR_HPP

#include <vector>

namespace city_plan {

    class Car {
    public:
        Car(size_t id, std::vector<size_t> path);

        size_t id() const;
        const std::vector<size_t> &path() const;

        friend std::ostream &operator<<(std::ostream &os, const Car &obj);

    private:
        const size_t id_;
        // path represented with street ids
        const std::vector<size_t> path_;
    };

}// namespace city_plan

#endif
