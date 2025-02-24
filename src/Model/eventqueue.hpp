//
// Created by farrukh on 24/02/25.
//

#ifndef EVENTQUEUE_HPP
#define EVENTQUEUE_HPP

#include <vector>
#include <queue>
#include "ControllerEnum.hpp"

class EventQueue {
public:
    void push(Action type) {
        queue_.push(type);
    }

    bool isEmpty() const {
        return queue_.empty();
    }

    Action pop() {
        Action type = queue_.front();
        queue_.pop();
        return type;
    }

private:
    std::queue<Action> queue_;
};
#endif //EVENTQUEUE_HPP
