//
// Created by farrukh on 26/02/25.
//

#include "GameController.hpp"

GameController::GameController() {}


Action GameController::getAction(KeyboardAction in) {
    bool controlsBlocked = areControlsBlocked();
    bool controlsInverted = areControlsInverted();
    Action action = Action::NONE;
    if(not controlsBlocked){
      if (not controlsInverted){
        switch (in) {
        	case KeyboardAction::LEFT_DIRECTION_KEY:
            	action = Action::MOVE_LEFT;
            	break;
        	case KeyboardAction::RIGHT_DIRECTION_KEY:
            	action = Action::MOVE_RIGHT;
            	break;
        	case KeyboardAction::LEFT_ROTATION_KEY:
            	action = Action::ROTATE_LEFT;
            	break;
        	case KeyboardAction::RIGHT_ROTATION_KEY:
            	action = Action::ROTATE_RIGHT;
            	break;
        	case KeyboardAction::HARD_DROP_KEY:
            	action = Action::DROP;
            	break;
        	case KeyboardAction::SOFT_DROP_KEY:
            	action = Action::SOFT_DROP;
            	break;
        	case KeyboardAction::EXIT_KEY:
            	action = Action::ESCAPE;
            	break;
        	default:
            	action = Action::NONE;
            	break;
        }
      }
      else{  //Si les commandes se font inverser par un malus
      	switch (in) {
        	case KeyboardAction::LEFT_DIRECTION_KEY:
            	action = Action::MOVE_RIGHT;
            	break;
        	case KeyboardAction::RIGHT_DIRECTION_KEY:
            	action = Action::MOVE_LEFT;
            	break;
        	case KeyboardAction::LEFT_ROTATION_KEY:
            	action = Action::ROTATE_RIGHT;
            	break;
        	case KeyboardAction::RIGHT_ROTATION_KEY:
            	action = Action::ROTATE_LEFT;
            	break;
        	case KeyboardAction::HARD_DROP_KEY:
            	action = Action::DROP;
            	break;
        	case KeyboardAction::SOFT_DROP_KEY:
            	action = Action::SOFT_DROP;
            	break;
        	case KeyboardAction::EXIT_KEY:
            	action = Action::ESCAPE;
            	break;
        	default:
            	action = Action::NONE;
            	break;
       }
     }
	 return action;
    }
	std::lock_guard<std::mutex> lock(queueMutex);
    eventQueue->addEvent(make_shared<MovementActionEvent>(action));
	return Action::NONE;
}

void GameController::applyMalusBonus(MalusBonus malBon, int target) {
    std::lock_guard<std::mutex> lock(queueMutex);
    eventQueue->addEvent(std::make_shared<MalusBonusEvent>(malBon, target));
}