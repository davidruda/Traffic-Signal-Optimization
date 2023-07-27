#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <optional>
#include <string>
#include <unordered_map>

namespace simulation {

    struct TimeInterval {
    public:
        TimeInterval(size_t begin, size_t end) : begin_(begin), end_(end) {}

        size_t begin() const {
            return begin_;
        }

        size_t end() const {
            return end_;
        }

        size_t duration() const {
            return end_ - begin_;
        }

    private:
        size_t begin_;
        size_t end_;
    };

    class Schedule {
    public:
        Schedule() : duration_(0) {}

        size_t length() const {
            return green_lights_.size();
        }

        size_t duration() const {
            return duration_;
        }

        const std::unordered_map<size_t, TimeInterval> &green_lights() const {
            return green_lights_;
        }

        void add_street(size_t street_id, size_t green_light_duration);
        std::optional<size_t> next_green(size_t street_id, size_t current_time);
        void reset();

    private:
        size_t duration_;
        std::unordered_map<size_t, TimeInterval> green_lights_;
    };

}

#endif
