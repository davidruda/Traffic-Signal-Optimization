#ifndef CITY_PLAN_INTERSECTION_HPP
#define CITY_PLAN_INTERSECTION_HPP

#include <vector>

namespace city_plan {

    class Intersection {
    public:
        explicit Intersection(size_t id)
            : id_(id), non_trivial_(false) {}

        size_t id() const {
            return id_;
        }

        const std::vector<size_t> &incoming() const {
            return incoming_streets_;
        }

        void add_incoming(size_t street_id) {
            incoming_streets_.emplace_back(street_id);
        }

        friend std::ostream &operator<<(std::ostream &os, const Intersection &obj);

    private:
        const size_t id_;
        // incoming streets represented by street ids
        std::vector<size_t> incoming_streets_;
        bool non_trivial_;
    };

}

#endif
