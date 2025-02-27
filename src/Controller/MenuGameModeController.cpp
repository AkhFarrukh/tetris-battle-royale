//
// Created by farrukh on 25/02/25.
//

#include "MenuGameModeController.hpp"



void MenuGameModeController::startEndlessMode() {
  //todo group this in a separate class
  shared_ptr<EventQueue> eventQueue = make_shared<EventQueue>();
  shared_ptr<Game> game = make_shared<Game>(eventQueue);
  shared_ptr<GameController> gameController = make_shared<GameController>(game, eventQueue);
  game->setGameController(gameController);
  game->run();  // Start the selected game mode
}