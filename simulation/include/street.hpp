#ifndef STREET_HPP
#define STREET_HPP

#include <functional>
#include <optional>
#include <queue>
#include <string>
#include <utility>

class Street {
public:
    Street(
            size_t id,
            size_t start_id,
            size_t end_id,
            std::string name,
            size_t length);

    const std::string &name() const;
    void set_used(bool used);

    friend std::ostream &operator<<(std::ostream &os, const Street &obj);

    class Instance;

private:
    const size_t id_;
    const size_t start_id_;
    const size_t end_id_;
    const std::string name_;
    const size_t length_;
    bool used_;
};

class Street::Instance {
public:
    explicit Instance(const Street &data);

    size_t car_queue_size() const;
    void add_car(size_t car_id);
    size_t get_car(size_t time);

    size_t id() const;
    size_t end() const;
    const std::string &name() const;
    size_t length() const;

    std::optional<size_t> last_used_time() const;
    bool is_used() const;

private:
    const Street &data_;
    // car queue represented by car ids
    std::queue<size_t> car_queue_;
    std::optional<size_t> last_used_time_;
};

#endif
