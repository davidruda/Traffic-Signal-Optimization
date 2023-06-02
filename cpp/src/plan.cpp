
#include <fstream>

#include "plan.hpp"

PlanInstance::PlanInstance(const SimulationInstance &simulation, const std::string &filename)
    : simulation_(simulation) {
    std::ifstream data_file{filename};
    int number_of_intersections;
    int intersection_id;
    int number_of_streets;
    std::string street_name;
    int green_light_time;

    data_file >> number_of_intersections;
    for (int i = 0; i < number_of_intersections; ++i) {
        data_file >> intersection_id;
        auto &&intersection_schedule = plan_.emplace(intersection_id, std::vector<std::ranges::iota_view<size_t>>{}).first->second;
        data_file >> number_of_streets;
        for (int j = 0; j < number_of_streets; ++j) {
            data_file >> street_name >> green_light_time;
            //simulation_.
        }
    }
}

//PlanInstance::PlanInstance(const PlanShared &data) : data_(data) {}

//void PlanInstance::create_default(const SimulationInstance &simulation, bool used) {
//    std::unordered_map<int, std::vector<std::ranges::iota_view<size_t>>> plan;
//    for (auto&& intersection:simulation.intersections()) {
//        auto&& streets = intersection.data().incoming();
//        auto&& [id, intersection_schedule] = *plan.emplace(intersection.data().id(), std::vector<std::ranges::iota_view<size_t>>{}).first;
//        for (size_t i = 0; i < streets.size(); ++i) {
//            auto&& t = std::views::iota(i, i + 1);
//            street_schedule.push_back(t);
//        }
//    }
//}
