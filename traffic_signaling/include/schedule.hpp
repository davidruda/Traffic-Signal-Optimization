#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>

namespace simulation {

    class Schedule {
    public:
        Schedule();
        size_t length() const;
        size_t duration() const;
        void add_street(size_t street_id, size_t green_light_duration);
        std::optional<size_t> next_green(size_t street_id, size_t current_time, std::optional<size_t> last_used_time);
        const std::unordered_map<size_t, std::ranges::iota_view<size_t, size_t>> &green_lights() const;
        void reset();

    private:
        size_t duration_;
        std::unordered_map<size_t, std::ranges::iota_view<size_t, size_t>> green_lights_;
    };

}

#endif
