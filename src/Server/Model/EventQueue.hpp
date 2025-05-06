//
// Created by farrukh on 27/02/25.
//

#ifndef EVENTQUEUE_HPP
#define EVENTQUEUE_HPP

#include <memory>
#include <queue>
#include "Event.hpp"

using namespace std;

class EventQueue {
private:
    queue<shared_ptr<Event>> eventQueue_;

public:
    void addEvent(shared_ptr<Event> event) {
        eventQueue_.push(event);
    }

    shared_ptr<Event> getEvent() {
      if (!eventQueue_.empty()) {
        shared_ptr<Event> event = eventQueue_.front();
        eventQueue_.pop();
        return event;
      }

      return nullptr;
    }

    bool isEmpty() {
        return eventQueue_.empty();
    }

};



#endif //EVENTQUEUE_HPP
