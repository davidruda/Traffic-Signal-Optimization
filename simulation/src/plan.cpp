
#include <fstream>

#include "plan.hpp"

//Plan::Instance::Instance(const Simulation::Instance &simulation, const std::string &filename)
//    : simulation_(simulation) {
//    std::ifstream data_file{filename};
//    int number_of_intersections;
//    int intersection_id;
//    int number_of_streets;
//    std::string street_name;
//    int green_light_duration;
//
//    data_file >> number_of_intersections;
//    for (int i = 0; i < number_of_intersections; ++i) {
//        data_file >> intersection_id;
//        auto &&intersection_schedule = plan_.emplace(intersection_id, intersection_id).first->second;
//        data_file >> number_of_streets;
//
//        for (int j = 0; j < number_of_streets; ++j) {
//            data_file >> street_name >> green_light_duration;
//            auto&& street = simulation.street_by_name(street_name);
//            intersection_schedule.add_street_to_schedule(street.id(), green_light_duration);
//        }
//    }
//}
//
//int Plan::Instance::next_green(int time, const Street::Instance &street) {
//    auto intersection_id = street.end().id();
//    auto&& schedule = plan_.at(intersection_id);
//
//    return 0;
//}
//
//Plan::Instance::Schedule::Schedule(int intersection_id)
//    : intersection_id_(intersection_id), duration_(0), last_used_time_(-1) {}
//
//int Plan::Instance::Schedule::duration() const {
//    return duration_;
//}
//
//void Plan::Instance::Schedule::add_street_to_schedule(int street_id, int green_light_duration) {
//    green_light_ranges_.emplace(street_id, std::views::iota(duration_, green_light_duration));
//    duration_ += green_light_duration;
//}
//
//int Plan::Instance::Schedule::next_green(int time, const Street::Instance &street) {
//    auto&& green_light_range = green_light_ranges_.at(street.id());
//    if (time <= last_used_time_) {
//
//    }
//}
