
#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>

#include "../Component/grid.cpp"
#include "../Controller/movingfigure.cpp"
#include "../Controller/Mouvement.cpp"
#include "../Model/game_data.hpp"
#include "../Controller/input.cpp"

#include "../Model/figureGenerator.cpp"
#include "../Controller/action.cpp"

class Game {
public:
    Game();
    ~Game();

    void run();
    void stop();

private:
    std::shared_ptr<Grid> grid;
    std::shared_ptr<MovingFigure> activeFigure;
    Mouvement movement;
    FigureGenerator figureGenerator;
    GameData gameData;
    Input inputHandler;
    bool isRunning;

    void processInput();
    void updateGame();
    void renderGame();
    void checkGameOver();
    void handleLineCompletion();
};

#endif // GAME_HPP
