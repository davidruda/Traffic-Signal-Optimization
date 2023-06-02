#ifndef CAR_HPP
#define CAR_HPP

#include <functional>
#include <utility>
#include <vector>

#include "street.hpp"

class StreetShared;

class CarShared {
public:
    CarShared(
            int id,
            int path_length,
            std::vector<std::reference_wrapper<const StreetShared>> path);

    int id() const;
    int path_length() const;
    const std::vector<std::reference_wrapper<const StreetShared>> &path() const;

    friend std::ostream &operator<<(std::ostream &os, const CarShared &obj);

private:
    const int id_;
    const int path_length_;
    const std::vector<std::reference_wrapper<const StreetShared>> path_;
};

class CarInstance {
public:
    explicit CarInstance(const CarShared &data);

    const StreetShared &current_street() const;
    void move_to_next_street();
    bool at_final_destination() const;

    const CarShared &data() const;
    void set_finished(bool finished);
    void set_finish_time(int finishTime);
    int finish_time() const;
    bool finished() const;


private:
    const CarShared &data_;

    size_t street_in_path_index_;
    //TODO: finished is redundant, finish_time is enough
    int finish_time_;
    bool finished_;
};

#endif
