#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "grid.cpp"
#include "movingfigure.cpp"
#include "game_data.hpp"
#include "input.cpp"


class Game {
public:
	Game();
//	Game(Game &&) = default;
//	Game(const Game &) = default;
//	Game &operator=(Game &&) = default;
//	Game &operator=(const Game &) = default;
	~Game();
  void run();
  void stop();


private:
  std::shared_ptr<Grid> grid;
  std::shared_ptr<Figure> activeFigure; 
  Mouvement movement;
  GameData gameData;
  Input inputHandler;
  bool isRunning;

  void processInput();
  void updateGame();
  void renderGame();
  void checkGameOver();
  void handleLineCompletion();

};

