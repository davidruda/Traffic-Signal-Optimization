#ifndef SIMULATION_SCHEDULE_HPP
#define SIMULATION_SCHEDULE_HPP

#include <cassert>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace simulation {

    struct TimeInterval {
    public:
        TimeInterval(size_t begin, size_t end) : begin_(begin), end_(end) {
            assert(begin <= end);
        }

        size_t begin() const {
            return begin_;
        }

        size_t end() const {
            return end_;
        }

        size_t duration() const {
            return end_ - begin_;
        }

        void set_interval(size_t begin, size_t end);

    private:
        size_t begin_;
        size_t end_;
    };

    class Schedule {
    public:
        Schedule() : total_duration_(0) {}

        size_t length() const {
            return green_lights_.size();
        }

        size_t duration() const {
            return total_duration_;
        }

        std::vector<std::pair<size_t, TimeInterval>> green_light_schedule() const;
        void set_schedule(const std::vector<size_t> &times, const std::vector<size_t> &order);
        void add_street(size_t street_id, size_t green_light_duration);
        std::optional<size_t> next_green(size_t street_id, size_t current_time);
        void reset();

    private:
        size_t total_duration_;
        std::unordered_map<size_t, size_t> street_index_;
        std::vector<TimeInterval> green_lights_;
    };

}

#endif
