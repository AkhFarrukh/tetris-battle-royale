
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


    void processInput();
    void updateGame();
    void renderGame();
    int applyColor(Color blockColor);
    void checkGameOver();
    void handleLineCompletion();
};

#endif // GAME_HPP
