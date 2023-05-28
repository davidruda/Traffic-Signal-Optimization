#ifndef CAR_INSTANCE_HPP
#define CAR_INSTANCE_HPP

#include "car_shared.hpp"
#include "street_instance.hpp"

class StreetInstance;

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
