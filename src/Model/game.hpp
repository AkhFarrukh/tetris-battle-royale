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
#include "serverFunctions.hpp"
#include "playerData.hpp"

class GameController; // forward declaration


class Game {
public:

    Game(std::vector<Server::ThreadGameData> playersData, int numPlayers, string gameMode);
    ~Game();

    void run();
    void stop();
    
    void setGameController(shared_ptr<GameController> controller);
    Color getBlockColor(Coordinates cell);
    void broadcastGame();


private:

    std::chrono::steady_clock::time_point lastDropTime;
    std::vector<PlayerState> players;
    FigureGenerator figureGenerator;
    bool isRunning;
    shared_ptr<GameController> gameController;
    string gameMode;



    void checkInput();

    void updateGame();

    void handleMovementActionEvent(int p, std::shared_ptr<Event> e);

    void renderGame();
    int  applyColor(Color blockColor);
    void checkGameOver();
    void handleLineCompletion(int p);

    void placePieceOnBoard(int p);


};

#endif // GAME_HPP
