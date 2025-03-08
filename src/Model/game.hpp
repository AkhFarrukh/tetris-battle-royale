
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
#include "MovementActionEvent.hpp"
#include "TimerDropEvent.hpp"
#include "userdatabasemanager.hpp"
#include "gameModes.h"

class Game {
public:
    Game(shared_ptr<EventQueue> eventQueue, int uid);
    ~Game();

    void run();
    void stop();



    void setGameController(shared_ptr<GameController> controller);
    int getGameScore() { return gameData.get_score(); }
    int getGameNumberOfBrokenLines() { return gameData.get_numberBrokenLines(); }
    Color getBlockColor(Coordinates cell);
	int getTimeDrop();
    void setTimeDrop(int newTime);
    int getTetriminoCounter();
    void setTetriminoCount(int& count);


private:
    int user_id; 
    std::shared_ptr<Grid> grid;
    FigureGenerator figureGenerator;
    std::shared_ptr<MovingFigure> activeFigure;
    Mouvement movement;
    GameData gameData;
    UserDatabaseManager userDatabaseManager;
    RoyalCompetition royalCompetition;
    bool isRunning;
    int timeDropEvent = 500;
    shared_ptr<GameController> gameController = nullptr;
    shared_ptr<EventQueue> eventQueue = nullptr;
    int countTetriminoGenerated = 0;



    void checkInput();

    void updateGame();

    void handleMovementActionEvent(shared_ptr<Event> event);

    void handleTimerDropEvent(shared_ptr<Event> event);

    void renderGame();
    int applyColor(Color blockColor);
    void checkGameOver();
    void handleLineCompletion();



};

#endif // GAME_HPP
