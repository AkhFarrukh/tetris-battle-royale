#ifndef MALUSBONUSEVENT_HPP
#define MALUSBONUSEVENT_HPP

#include "Event.hpp"
#include "ControllerEnum.hpp" // Action

class MalusBonusEvent : public Event {
    private:
        MalusBonus malusBonus;
        int target;
        
    public:
        MalusBonusEvent(MalusBonus malBon, int target)
          : malusBonus(malBon), target(target)
        {
        }
        // Provide a function to retrieve the Action
        MalusBonus getMalusBonus() const {
            return malusBonus;
        }
        int getTarget() const {
            return target;
        }
    };
    



#endif 
