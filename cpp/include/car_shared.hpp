#ifndef CAR_SHARED_HPP
#define CAR_SHARED_HPP

#include <utility>
#include <vector>

#include "street_shared.hpp"

class CarShared {
public:
    CarShared(
        const int id,
        const int path_length,
        std::vector<const StreetShared *> path
    ) : id_(id), path_length_(path_length), path_(std::move(path)) {}

private:
    const int id_;
    const int path_length_;
    const std::vector<const StreetShared *> path_;
};

#endif
