#ifndef STABLE_PRIORITY_QUEUE_HPP
#define STABLE_PRIORITY_QUEUE_HPP

#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>

/*
template<
        class T,
        class Container = std::vector<T>,
        class Compare = std::less<typename Container::value_type>>
class StablePriorityQueue {
public:
    StablePriorityQueue();
    explicit StablePriorityQueue(const Compare& compare);
    StablePriorityQueue(const Compare& compare, const Container& cont);
    StablePriorityQueue(const Compare& compare, Container&& cont);

    bool empty() const;
    size_t size() const;
    void push(const T& value);
    void push(T&& value);
    void pop();
    const T& top() const;

    template<class... Args>
    void emplace(Args&&... args);

private:
    std::priority_queue<T, Container, Compare> queue_;
    size_t counter_;
};

template<class T, class Container, class Compare>
inline StablePriorityQueue<T, Container, Compare>::StablePriorityQueue()
    : queue_(Compare(), Container()) {}

template<class T, class Container, class Compare>
inline StablePriorityQueue<T, Container, Compare>::StablePriorityQueue(const Compare& compare)
    : queue_(compare, Container()) {}

template<class T, class Container, class Compare>
inline StablePriorityQueue<T, Container, Compare>::StablePriorityQueue(const Compare& compare, const Container& cont)
    : queue_(compare, cont) {}

template<class T, class Container, class Compare>
inline StablePriorityQueue<T, Container, Compare>::StablePriorityQueue(const Compare& compare, Container&& cont)
    : queue_(compare, std::move(cont)) {}

template<class T, class Container, class Compare>
inline bool StablePriorityQueue<T, Container, Compare>::empty() const {
    return queue_.empty();
}

template<class T, class Container, class Compare>
inline size_t StablePriorityQueue<T, Container, Compare>::size() const {
    return queue_.size();
}

template<class T, class Container, class Compare>
inline void StablePriorityQueue<T, Container, Compare>::push(const T &value) {
    if (counter_ == std::numeric_limits<size_t>::max()) {
        throw std::overflow_error{"StablePriorityQueue counter overflow"};
    }
    queue_.push(std::make_pair(value, ++counter_));
}

template<class T, class Container, class Compare>
inline void StablePriorityQueue<T, Container, Compare>::push(T &&value) {
    if (counter_ == std::numeric_limits<size_t>::max()) {
        throw std::overflow_error{"StablePriorityQueue counter overflow"};
    }
    queue_.push(std::make_pair(std::move(value), ++counter_));
}

template<class T, class Container, class Compare>
inline void StablePriorityQueue<T, Container, Compare>::pop() {
    queue_.pop();
}

template<class T, class Container, class Compare>
inline const T &StablePriorityQueue<T, Container, Compare>::top() const {
    return queue_.top().first;
}

template<class T, class Container, class Compare>
template<class... Args>
inline void StablePriorityQueue<T, Container, Compare>::emplace(Args &&...args) {
    if (counter_ == std::numeric_limits<size_t>::max()) {
        throw std::overflow_error{"StablePriorityQueue counter overflow"};
    }
    queue_.emplace(std::forward<Args>(args)..., ++counter_);
}
*/

#endif
