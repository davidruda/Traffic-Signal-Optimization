#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <functional>
#include <optional>
#include <vector>

class Street;

//#include "street.hpp"
#include "schedule.hpp"

class Intersection {
public:
    explicit Intersection(size_t id);

    size_t id() const;

    void add_incoming(const Street &street);

    //const std::vector<std::reference_wrapper<const Street>> &incoming() const;

    friend std::ostream &operator<<(std::ostream &os, const Intersection &obj);

    class Instance;

private:
    const size_t id_;
    std::vector<std::reference_wrapper<const Street>> incoming_;
};

class Intersection::Instance {
public:
    explicit Instance(const Intersection &data);

    bool has_schedule() const;
    void reset_schedule();
    std::optional<std::reference_wrapper<const Schedule>> schedule() const;
    void add_street_to_schedule(size_t street_id, size_t green_light_duration);
    std::optional<size_t> next_green(size_t time, const Street::Instance &street);

    size_t id() const;
    const std::vector<std::reference_wrapper<const Street>> &incoming() const;

private:
    const Intersection &data_;

    Schedule schedule_;
};

#endif
