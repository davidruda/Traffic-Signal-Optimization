#ifndef CAR_HPP
#define CAR_HPP

#include <functional>
#include <utility>
#include <vector>

class Car {
public:
    Car(
            size_t id,
            size_t path_length,
            std::vector<size_t> path);

    friend std::ostream &operator<<(std::ostream &os, const Car &obj);

    class Instance;

private:
    const size_t id_;
    const size_t path_length_;
    // path represented with street ids
    const std::vector<size_t> path_;
};

class Car::Instance {
public:
    explicit Instance(const Car &data);

    size_t current_street() const;
    void move_to_next_street();
    bool at_final_destination() const;

    size_t id() const;

    void set_finished(bool finished);
    void set_finish_time(size_t finishTime);
    size_t finish_time() const;
    bool finished() const;

private:
    const Car &data_;

    size_t path_index_;
    size_t finish_time_;
    bool finished_;
};

#endif
