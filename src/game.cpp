#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

#include "input.cpp"
#include "grid.cpp"

class Game{
private:
    Input input;
    bool gameRunning = false;
    std::chrono::time_point<std::chrono::steady_clock> lastUpdateTime;
    //todo Movement movement;
    //todo NextFigure figureGenerator;
    //todo MovingFigure currentFigure;
    Grid grid;
    int level = 0;
    int moveInterval = 800; // in milliseconds todo should scale by level

    void gameLoop();
    void getElapsedTime();

public:
    Game();
    void run();
};



Game::Game() {
  run();
}

void Game::run() {
  gameRunning = true;
  //todo currentFigure = MovingFigure(figureGenerator->getFigure(); //get new figure
  //todo movement = Movement(); some constructor
  input.start();
  lastUpdateTime = std::chrono::steady_clock::now();
  gameLoop();
}

void Game::gameLoop() {
  while (gameRunning) {
    //todo movement->handleAction(input->getaction());
    if (getElapsedTime() >= moveInterval){
      //todo move figure down
      lastUpdateTime =  std::chrono::steady_clock::now();//current time
      //todo if not possible to move down
          //todo put figure blocks on grid
          //todo currentFigure = MovingFigure(figureGenerator->getFigure();
          //todo also check if we can spawn new figure
          gird->clearFullLines();
          //todo somehow add score based on how many lines cleared
    }
    //todo print interface
  }
}

long Game::getElapsedTime(){
  //checks how much time have passed from the last tick to move the figure down
  //
  auto currentTime= std::chrono::steady_clock::now();
  auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
          currentTime - lastUpdateTime).count();
}



