//
// Created by farrukh on 27/02/25.
//

#ifndef MOVEMENTACTIONEVENT_HPP
#define MOVEMENTACTIONEVENT_HPP

#include "Event.hpp"
#include "ControllerEnum.hpp" // Action

class MovementActionEvent: public Event{
    Action action;
public:
    MovementActionEvent(Action action):action{action}{};
    Action getEventData() {return action;}
};



#endif //MOVEMENTACTIONEVENT_HPP
