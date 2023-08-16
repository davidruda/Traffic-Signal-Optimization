#ifndef CITY_PLAN_STREET_HPP
#define CITY_PLAN_STREET_HPP

#include <string>

namespace city_plan {

    class Street {
    public:
        Street(size_t id, size_t start_id, size_t end_id, std::string name, size_t length)
            : id_(id), start_id_(start_id), end_id_(end_id), name_(std::move(name)), length_(length), used_(false) {}

        size_t id() const {
            return id_;
        }

        size_t end() const {
            return end_id_;
        }

        const std::string &name() const {
            return name_;
        }

        void set_used(bool used) {
            used_ = used;
        }

        bool is_used() const {
            return used_;
        }

        size_t length() const {
            return length_;
        }

        friend std::ostream &operator<<(std::ostream &os, const Street &obj);

    private:
        const size_t id_;
        const size_t start_id_;
        const size_t end_id_;
        const std::string name_;
        const size_t length_;
        bool used_;
    };

}

#endif
