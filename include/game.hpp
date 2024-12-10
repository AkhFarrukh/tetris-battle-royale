
#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib>

#include "Mouvement.hpp"
#include "game_data.hpp"
#include "input.hpp"

#include "figureGenerator.hpp"

class Game {
public:
    Game();
    ~Game();

    void run();
    void stop();

private:
    std::shared_ptr<Grid> grid;
    FigureGenerator figureGenerator;
    std::shared_ptr<MovingFigure> activeFigure;
    Mouvement movement;
    GameData gameData;
    bool isRunning;
    Input inputHandler;

    void processInput();
    void updateGame();
    void renderGame();
    void checkGameOver();
    void handleLineCompletion();
};

#endif // GAME_HPP
