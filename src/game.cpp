#include <iostream>
#include <thread>
#include <atomic>

#include "input.cpp"

class Game{
private:
    Input input;
    bool gameRunning = false;
    //todo Movement movement;
    //todo NextFigure figureGenerator;
    //todo MovingFigure currentFigure;
    void gameLoop();

public:
    Game();
    void run();


};



Game::Game() {
  run();
}

void Game::run() {
  gameRunning = true;
  //todo currentFigure = MovingFigure(figureGenerator->getFigure();
  input.start();
  gameLoop();
}

void Game::gameLoop(){
  while (gameRunning){
    //todo movement->handleAction(input->getaction());

  }
}

