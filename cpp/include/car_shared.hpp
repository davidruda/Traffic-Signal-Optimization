#ifndef CAR_SHARED_HPP
#define CAR_SHARED_HPP

#include <utility>
#include <vector>
#include <functional>

#include "street_shared.hpp"

class CarShared {
public:
    CarShared(
            int id,
            int path_length,
            std::vector<std::reference_wrapper<const StreetShared>> path
            );

    int id() const;
    int path_length() const;
    const std::vector<std::reference_wrapper<const StreetShared>> &path() const;

    friend std::ostream& operator<<(std::ostream& os, const CarShared& obj);

private:
    const int id_;
    const int path_length_;
    const std::vector<std::reference_wrapper<const StreetShared>> path_;
};

#endif
