#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <functional>
#include <optional>
#include <vector>

class Intersection {
public:
    explicit Intersection(size_t id);

    void add_incoming(size_t street_id);

    friend std::ostream &operator<<(std::ostream &os, const Intersection &obj);

    class Instance;

private:
    const size_t id_;
    // incoming streets represented by street ids
    //TODO: maybe change to unordered_set ?
    std::vector<size_t> incoming_streets_;
};

class Intersection::Instance {
public:
    explicit Instance(const Intersection &data);

    size_t id() const;
    const std::vector<size_t> &incoming() const;

private:
    const Intersection &data_;

    //Schedule schedule_;
};

#endif
