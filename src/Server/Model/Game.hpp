#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib>

#include "ServerFunctions.hpp"
#include "Mouvement.hpp"
#include "GameData.hpp"
#include "FigureGenerator.hpp"
#include "GameController.hpp"
#include "EventQueue.hpp"
#include "MovementActionEvent.hpp"
#include "TimerDropEvent.hpp"
#include "ServerData.hpp"
#include "PlayerData.hpp"
#include "Communication.hpp"
#include "MalusBonusEvent.hpp"
#include "GameModes.h"
#include "UserDatabaseManager.hpp"


using namespace std;

class GameController; // forward declaration
class Server;
class Mode;


class Game {
public:

    Game();
    Game(std::vector<ThreadGameData> playersData, int numPlayers, Modes gameMode, std::shared_ptr<ClientSession> clientSession = nullptr);
    ~Game();

    void run();
    void stop();
    
    bool broadcastGame(bool endGame = false);
    void setSpectators(const std::vector<int>* spectPtrs);


private:

    bool isRunning;
    int numPlayers_;
    Modes gameMode;
    std::shared_ptr<ClientSession> clientSes;
    std::vector<PlayerState> players;
    FigureGenerator figureGenerator;
    shared_ptr<Mode> mode;
    
    UserDatabaseManager udbm;
    const std::vector<int>* spectatorFdsPtr = nullptr;

    void updateGame();

    void handleMovementActionEvent(std::shared_ptr<Event> e, int p);
    void handleMalusBonusEvent(std::shared_ptr<Event> e, int p);
    void handleMalus(int targetID, int playerID);

    int  applyColor(Color blockColor);
    void checkGameOver();
    void handleLineCompletion(int p);

    void placePieceOnBoard(int p);
};

#endif // GAME_HPP
