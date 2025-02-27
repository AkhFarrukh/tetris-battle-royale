
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
#include "GameController.hpp"
#include "EventQueue.hpp"

class GameController; // forward declaration

class Game {
public:
    Game(shared_ptr<EventQueue> eventQueue);
    ~Game();

    void run();
    void stop();
    void setGameController(shared_ptr<GameController> controller);
    int getGameScore() { return gameData.get_score(); }
    int getGameNumberOfBrokenLines() { return gameData.get_numberBrokenLines(); }
    Color getBlockColor(Coordinates cell);



private:
    std::shared_ptr<Grid> grid;
    FigureGenerator figureGenerator;
    std::shared_ptr<MovingFigure> activeFigure;
    Mouvement movement;
    GameData gameData;
    bool isRunning;
    shared_ptr<GameController> gameController;
    shared_ptr<EventQueue> eventQueue;


    void updateGame();
    void renderGame();
    int applyColor(Color blockColor);
    void checkGameOver();
    void handleLineCompletion();


};

#endif // GAME_HPP
