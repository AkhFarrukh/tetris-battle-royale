//
// Created by farrukh on 28/02/25.
//

#ifndef TIMERDROPEVENT_HPP
#define TIMERDROPEVENT_HPP

#include "Event.hpp"
#include "ControllerEnum.hpp" // Action


class TimerDropEvent : public Event {
private:
  Action action = SOFT_DROP;
public:
    Action getEventData() {return action;}
};



#endif //TIMERDROPEVENT_HPP
