#ifndef SIMULATION_SCHEDULE_HPP
#define SIMULATION_SCHEDULE_HPP

#include <optional>
#include <unordered_map>
#include <vector>
#include <ranges>
#include <limits>
#include <random>

#include "city_plan/intersection.hpp"

namespace simulation {
/**
 * Schedule of traffic lights for one intersection in the simulation.
 */
class Schedule {
public:
    /**
     * Construct an empty schedule for the given intersection.
     *
     * @param intersection Intersection for which the schedule is created.
     */
    explicit Schedule(const city_plan::Intersection &intersection) : intersection_(intersection) {}

    /**
     * Construct a schedule using the order and times for the given intersection.
     *
     * @param intersection Intersection for which the schedule is created.
     * @param order Order of streets in the schedule.
     * @param times Green light times for each street in the order.
     * @param relative_order If True, `order` must be a vector of street indices relative to the intersection.
     * Otherwise, `order` must be a vector of street IDs.
     */
    Schedule(
        const city_plan::Intersection &intersection,
        std::vector<unsigned long> &&order, std::vector<unsigned long> &&times, bool relative_order = false
    );

    /**
     * Return the number of streets in the schedule.
     */
    unsigned long length() const {
        return static_cast<unsigned long>(green_lights_.size());
    }

    /**
     * Return the cycle duration of this schedule.
     */
    unsigned long duration() const {
        return total_duration_;
    }

    /**
     * Return the order of streets in the schedule.
     *
     * The streets are represented by their IDs.
     */
    const std::vector<unsigned long> &order() const {
        return order_;
    }

    /**
     * Return the green light times for each street in the order.
     */
    const std::vector<unsigned long> &times() const {
        return times_;
    }

    /**
     * Return the order of streets in the schedule.
     *
     * The streets are represented by their indices relative to the intersection.
     *
     * This method is only valid if the schedule contains used streets.
    */
    auto relative_order() const {
        return order_ | std::views::transform([&](unsigned long street_id) {
            return intersection_.street_index(street_id);
        });
    }

    /** Order initialization options for the schedule. */
    enum class Order {
        DEFAULT,
        ADAPTIVE,
        RANDOM,
        CUSTOM,
    };

    /** Times initialization options for the schedule. */
    enum class Times {
        DEFAULT,
        SCALED,
        CUSTOM,
    };

    /**
     * Set the schedule using the given order and times.
     *
     * @param order Order of streets in the schedule.
     * @param times Green light times for each street in the order.
     * @param relative_order If True, `order` must be a vector of street indices relative to the intersection.
     * Otherwise, `order` must be a vector of street IDs.
     */
    void set(std::vector<unsigned long> &&order, std::vector<unsigned long> &&times, bool relative_order = false);

    /**
     * Set the schedule using the given order and times initialization options.
     *
     * @param order_type Order initialization to use.
     * @param times_type Times initialization to use.
     */
    void set(Order order_type = Order::DEFAULT, Times times_type = Times::DEFAULT);

    /**
     * Reset the schedule to its initial state.
     *
     * This method is used for performance reasons to avoid frequent reallocation.
     */
    void reset();

    /**
     * Set the divisor for scaled times option.
     *
     * @param divisor Divisor to use.
     */
    static void set_divisor(unsigned long divisor) {
        if (divisor == 0) {
            throw std::invalid_argument{"divisor cannot be zero"};
        }
        divisor_ = divisor;
    }

    /**
     * Return the next time the green light will be on for the given street.
     *
     * @param street_id ID of the street to check.
     * @param time Current time.
     */
    std::optional<unsigned long> next_green(unsigned long street_id, unsigned long time);

    /** Helper function to fill in the missing streets when using adaptive order option. */
    void fill_missing_streets();

    /* Default divisor for the scaled times option. */
    static constexpr auto DEFAULT_DIVISOR = 27UL;

private:
    using TimeInterval = std::ranges::iota_view<unsigned long, unsigned long>;

    /** Initialize the adaptive order option. */
    void set_adaptive();
    /** Add a not yet scheduled street to the schedule when using adaptive order option. */
    void add_street_adaptive(unsigned long street_id, unsigned long time);

    /** Intersection for which this schedule is. */
    const city_plan::Intersection &intersection_;
    /** Whether the schedule uses the adaptive order option. */
    bool adaptive_{};
    /** The cycle duration of this schedule in seconds. */
    unsigned long total_duration_{};

    /** Order of streets in the schedule (streets are represented by their IDs). */
    std::vector<unsigned long> order_;
    /** Green light times for each street in the order. */
    std::vector<unsigned long> times_;

    /**
     * Green light intervals for each street indexed by street ID.
     */
    std::unordered_map<unsigned long, TimeInterval> green_lights_;

    /** Divisor for the scaled times option. */
    static unsigned long divisor_;
    /**
     * A constant representing an unused slot in the `order_` vector.
     *
     * Only used by the adaptive order option.
     */
    static constexpr auto UNUSED = std::numeric_limits<unsigned long>::max();
};

/**
 * Set the random seed used for schedules generation.
 *
 * @param seed Value of the random seed.
 */
void set_seed(unsigned long seed);

/// Use this exact one deterministic random shuffle algorithm (Fisher–Yates Knuth shuffle) instead of relying on
/// different implementations of `std::shuffle` across different compilers and std libraries producing different results.
///
/// However, this function is still not totally platform independent because it relies on `std::uniform_int_distribution` implementation.
/// On Windows, it produces the same results with MSVC, GCC and Clang.
/// On Linux, it produces the same results with GCC and Clang but only with `-stdlib=libstdc++` - not with `-stdlib=libc++`.
///
/// Source: https://en.cppreference.com/w/cpp/algorithm/random_shuffle#Version_3
template<class RandomIt, class URBG>
void deterministic_shuffle(RandomIt first, RandomIt last, URBG &&g){
    using diff_t = std::iterator_traits<RandomIt>::difference_type;
    using distr_t = std::uniform_int_distribution<diff_t>;
    using param_t = distr_t::param_type;

    distr_t D;
    for (diff_t i = last - first - 1; i > 0; --i) {
        std::swap(first[i], first[D(g, param_t(0, i))]);
    }
}
}

#endif
