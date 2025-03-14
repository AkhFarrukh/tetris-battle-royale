//
// Created by farrukh on 27/02/25.
//

#ifndef MOVEMENTACTIONEVENT_HPP
#define MOVEMENTACTIONEVENT_HPP

#include "Event.hpp"
#include "ControllerEnum.hpp" // Action

class MovementActionEvent : public Event {
    private:
        int playerId;
        Action action;
    public:
        MovementActionEvent(Action act)
          : action(act)
        {
        }
        // Provide a function to retrieve the Action
        Action getEventData() const {
            return action;
        }
    };
    



#endif //MOVEMENTACTIONEVENT_HPP
