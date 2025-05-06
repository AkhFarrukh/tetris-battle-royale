#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib>

#include "serverFunctions.hpp"
#include "Mouvement.hpp"
#include "game_data.hpp"
#include "figureGenerator.hpp"
#include "GameController.hpp"
#include "EventQueue.hpp"
#include "MovementActionEvent.hpp"
#include "TimerDropEvent.hpp"
#include "ServerData.hpp"
#include "playerData.hpp"
#include "communication.hpp"
#include "MalusBonusEvent.hpp"
#include "gameModes.h"
#include "userdatabasemanager.hpp"


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
    
    Color getBlockColor(Coordinates cell);
    bool broadcastGame(bool endGame = false);
    void setSpectators(const std::vector<int>* spectPtrs);


private:

    std::shared_ptr<ClientSession> clientSes;
    Modes gameMode;
    std::vector<PlayerState> players;
    FigureGenerator figureGenerator;
    bool isRunning;
    shared_ptr<Mode> mode;
    int numPlayers_;
    UserDatabaseManager udbm;
    const std::vector<int>* spectatorFdsPtr = nullptr;



    void checkInput();

    void updateGame();

    void handleMovementActionEvent(std::shared_ptr<Event> e, int p);
    void handleMalusBonusEvent(std::shared_ptr<Event> e, int p);
    void handleMalus(int targetID, int playerID);

    void renderGame();
    int  applyColor(Color blockColor);
    void checkGameOver();
    void handleLineCompletion(int p);

    void placePieceOnBoard(int p);
    



};

#endif // GAME_HPP
