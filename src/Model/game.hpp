
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

class GameController; // forward declaration

class Game {
public:

    Game(shared_ptr<EventQueue> eventQueue, int numPlayers);
    ~Game();

    void run();
    void stop();
    



    void setGameController(shared_ptr<GameController> controller);
    int getGameScore() { return gameData.get_score(); }
    int getGameNumberOfBrokenLines() { return gameData.get_numberBrokenLines(); }
    Color getBlockColor(Coordinates cell);



private:
struct PlayerState {
    std::shared_ptr<Grid>            grid;
    std::shared_ptr<MovingFigure>    activeFigure;
    Mouvement                         movement;
    GameData                         gameData;
    
    std::shared_ptr<Figure>          nextTetromino;

    int                               playerId;
    bool                              isGameOver = false;
    
    PlayerState(int pid)
        : playerId(pid), movement(Action::NONE, nullptr), gameData(0, 0, false, "Royal")
    {}  
    };
    
    std::chrono::steady_clock::time_point lastDropTime;
    std::vector<PlayerState> players;
    std::shared_ptr<Grid> grid;
    FigureGenerator figureGenerator;
    std::shared_ptr<MovingFigure> activeFigure;
    Mouvement movement;
    GameData gameData;
    bool isRunning;
    shared_ptr<GameController> gameController;
    shared_ptr<EventQueue> eventQueue;



    void checkInput();

    void updateGame();

    void handleMovementActionEvent(shared_ptr<Event> event);

    void handleTimerDropEvent(shared_ptr<Event> event);

    void renderGame();
    int applyColor(Color blockColor);
    void checkGameOver();
    void handleLineCompletion(int p);

    void placePieceOnBoard(int p);


};

#endif // GAME_HPP
