//
// Created by farrukh on 26/02/25.
//

#include "GameController.hpp"

GameController::GameController(shared_ptr<EventQueue> eventQueue) : eventQueue(eventQueue) {}


void GameController::getAction(KeyboardAction in) {
    bool controlsBlocked = areControlsBlocked();
    bool controlsInverted = areControlsInverted();
    Action action = Action::NONE;
    if(not controlsBlocked){
      if (not controlsInverted){
        switch (keyboardAction) {
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
      	switch (keyboardAction) {
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
    }
    eventQueue->addEvent(make_shared<MovementActionEvent>(action));
}

/*
int GameController::getGameScore() {
    return game->getGameScore();
}

int GameController::getGameNumberOfBrokenLines() {
    return game->getGameNumberOfBrokenLines();
}

Color GameController::getBlockColor(Coordinates cell) {
    return game->getBlockColor(cell);
}
*/
/*
void GameController::renderGame() {
    RenderEngineNcurses::renderGame(game);
}

void GameController::printGameOver() {
    RenderEngineNcurses::printGameOver();
}
*/