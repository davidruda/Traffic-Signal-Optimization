#ifndef PLAN_HPP
#define PLAN_HPP


#include <ranges>
#include <string>
#include <unordered_map>

#include "simulation.hpp"

class Plan {
public:
    class Instance;
};

class Plan::Instance {
public:
    explicit Instance(const Simulation::Instance &simulation, const std::string &filename);
    //explicit PlanInstance(const Plan &data);

    void create_default(const Simulation::Instance &simulation, bool used);

    int next_green(int time, const Street::Instance &street);

    //class Schedule {
    //public:
    //    explicit Schedule(int intersection_id);
    //    int duration() const;
    //    void add_street_to_schedule(int street_id, int green_light_duration);
    //    int next_green(int time, const Street::Instance &street);
    //private:
    //    int intersection_id_;
    //    int duration_;
    //    int last_used_time_;
    //    std::unordered_map<int, std::ranges::iota_view<int, int>> green_light_ranges_;
    //};

private:
    const Simulation::Instance &simulation_;
    //const Plan &data_;

    //std::unordered_map<int, Schedule> plan_;
};


#endif
