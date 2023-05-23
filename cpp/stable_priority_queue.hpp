#ifndef STABLE_PRIORITY_QUEUE_HPP
#define STABLE_PRIORITY_QUEUE_HPP

#include <queue>

template<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
>
class StablePriorityQueue {
public:

private:
    std::priority_queue<T, Container, Compare> queue;
    size_t counter;
};

#endif
