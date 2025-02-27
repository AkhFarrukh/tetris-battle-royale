//
// Created by farrukh on 26/02/25.
//

#include "GameController.hpp"

GameController::GameController(shared_ptr<Game> game) : game(game) {}


Action GameController::getAction() {
    KeyboardAction keyboardAction = input.getKeyboardInput();
    Action action = Action::NONE;
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
    return action;
}


int GameController::getGameScore() {
    return game->getGameScore();
}

int GameController::getGameNumberOfBrokenLines() {
    return game->getGameNumberOfBrokenLines();
}

Color GameController::getBlockColor(Coordinates cell) {
    return game->getBlockColor(cell);
}