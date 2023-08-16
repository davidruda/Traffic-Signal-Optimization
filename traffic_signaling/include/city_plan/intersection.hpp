#ifndef CITY_PLAN_INTERSECTION_HPP
#define CITY_PLAN_INTERSECTION_HPP

#include <vector>

namespace city_plan {

    class Intersection {
    public:
        explicit Intersection(size_t id)
            : id_(id), used_(false), non_trivial_(false) {}

        void set_used(bool used) {
            used_ = used;
        }

        bool is_used() const {
            return used_;
        }

        void set_non_trivial(bool non_trivial) {
            non_trivial_ = non_trivial;
        }

        bool is_non_trivial() const {
            return non_trivial_;
        }

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
        bool used_;
        bool non_trivial_;
    };

}

#endif
