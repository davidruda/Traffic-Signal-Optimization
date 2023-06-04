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
    explicit Intersection(int id);

    int id() const;

    void add_incoming(const Street &street);

    void add_outgoing(const Street &street);

    const std::vector<std::reference_wrapper<const Street>> &incoming() const;

    friend std::ostream &operator<<(std::ostream &os, const Intersection &obj);

    class Instance;

private:
    const int id_;
    std::vector<std::reference_wrapper<const Street>> incoming_;
    //TODO: outgoing is never needed and therefore redundant
    std::vector<std::reference_wrapper<const Street>> outgoing_;
};

class Intersection::Instance {
public:
    explicit Instance(const Intersection &data);

    void add_street_to_schedule(int street_id, int green_light_duration);
    std::optional<int> next_green(int time, const Street::Instance &street);

    int id() const;
    const std::vector<std::reference_wrapper<const Street>> &incoming() const;

private:
    const Intersection &data_;

    Schedule schedule_;
};

#endif
