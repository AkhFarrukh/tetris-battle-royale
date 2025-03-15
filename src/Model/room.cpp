#include "room.hpp"
#include "game.hpp"


Room::Room(){}

Room::Room(int sockHoster, Modes mode)
    : sockHoster(sockHoster),
      gameMode(mode),
      matchStarted(false),
      roomID(sockHoster)
{   
    if (mode == Modes::Duel_){
        minPlayers = 2;
        maxPlayers = 2;
    }
    playerFds.push_back(sockHoster);
    ThreadGameData thread;
    eventQueue = std::make_shared<EventQueue>();
    thread.client_fd = sockHoster;
    thread.eventQueue = eventQueue;
    playersThread.push_back(thread);
}

Room::~Room() {
}

bool Room::addPlayer(int playerFd) {
    std::unique_lock<std::mutex> lock(mtx);

    if (matchStarted || (int)playerFds.size() >= maxPlayers) {
        return false;
    }
    ThreadGameData thread;
    eventQueue = std::make_shared<EventQueue>(); //event queue different pour chaque client?
    thread.client_fd = playerFd;
    thread.eventQueue = eventQueue;
    playersThread.push_back(thread);
    playerFds.push_back(playerFd);
    std::cout << "Player joined room: fd=" << playerFd 
              << " (now " << playerFds.size() << " players)" << std::endl;

    /*if ((int)playerFds.size() >= maxPlayers) {
        std::cout << "Room is full, automatically starting match..." << std::endl;
        matchStarted = true;
        launchGame();
        cv.notify_all();
    }*/
    cv.notify_all();
    return true;
}

int Room::getPlayerCount() const {
    std::lock_guard<std::mutex> lock(mtx);
    return (int)playerFds.size();
}

bool Room::isStarted() {
    return matchStarted;
}

void Room::forceStart() {
    std::unique_lock<std::mutex> lock(mtx);
    if (!matchStarted) {
        if (playerFds.size() >= (size_t)minPlayers) {
            matchStarted = true;
            launchGame();
            cv.notify_all();
        } else {
            std::cout << "Cannot force start, not enough players.\n";
        }
    }
}

bool Room::checkGameReady(){
    if ((int)playerFds.size() >= maxPlayers){return true;}
    return false;
}

std::shared_ptr<Game> Room::launchGame() {
    std::cout << "Launching game with " << playerFds.size() << " players\n";
    
    int numPlayers = (int)playerFds.size();
    std::string mode = "Endless";
    gamePtr = std::make_shared<Game>(std::vector<ThreadGameData>(playersThread), int(numPlayers), std::string(mode));

    // gamePtr->setPlayerFDs(playerFds);
    /*
    std::thread gameThread([this] {
        gamePtr->run();
    });
    gameThread.detach();
    */
}
