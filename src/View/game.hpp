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

  std::unique_ptr<Grid> grid;
  std::unique_ptr<MovingFigure> activeFigure;
  GameData gameData;
  Input inputHandler;
  bool isRunning;

  void processInput();
  void updateGame();
  void renderGame();
  void checkGameOver();
  void handleLineCompletion();
 
};


