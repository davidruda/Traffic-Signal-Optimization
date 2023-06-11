#ifndef CAR_HPP
#define CAR_HPP

#include <functional>
#include <utility>
#include <vector>

class Street;

//#include "street.hpp"


class Car {
public:
    Car(
            size_t id,
            size_t path_length,
            std::vector<std::reference_wrapper<const Street>> path);

    size_t id() const;
    //size_t path_length() const;
    //const std::vector<std::reference_wrapper<const Street>> &path() const;

    friend std::ostream &operator<<(std::ostream &os, const Car &obj);

    class Instance;

private:
    const size_t id_;
    const size_t path_length_;
    const std::vector<std::reference_wrapper<const Street>> path_;
};

class Car::Instance {
public:
    explicit Instance(const Car &data);

    const Street &current_street() const;
    void move_to_next_street();
    bool at_final_destination() const;

    size_t id() const;
    //size_t path_length() const;
    //const std::vector<std::reference_wrapper<const Street>> &path() const;

    void set_finished(bool finished);
    void set_finish_time(size_t finishTime);
    size_t finish_time() const;
    bool finished() const;

private:
    const Car &data_;

    size_t street_in_path_index_;
    size_t finish_time_;
    bool finished_;
};

#endif
