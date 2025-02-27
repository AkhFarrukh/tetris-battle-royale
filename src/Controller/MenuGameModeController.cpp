//
// Created by farrukh on 25/02/25.
//

#include "MenuGameModeController.hpp"


void MenuGameModeController::startEndlessMode() {
  //todo group this in a separate class
    shared_ptr<Game> game = make_shared<Game>();
    shared_ptr<GameController> gameController = make_shared<GameController>(game);
    game->setGameController(gameController);
    game->run();  // Start the selected game mode
}