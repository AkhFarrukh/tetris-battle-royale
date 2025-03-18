#include "room.hpp"
#include "game.hpp"


Room::Room(){}

Room::Room(int sockHoster, Modes mode, std::shared_ptr<EventQueue> hostEventQueue)
    : sockHoster(sockHoster),
      gameMode(mode),
      matchStarted(false),
      roomID(sockHoster)
{   
    if (mode == Modes::Duel_){
        minPlayers = 2;
        maxPlayers = 2;
    }
    if (mode == Modes::Royal_ or mode == Modes::Classic_){
        minPlayers = 3;
        maxPlayers = 9;
    }
    gamePtr = nullptr;
    playerFds.push_back(sockHoster);
    ThreadGameData thread;
    eventQueue = std::make_shared<EventQueue>();
    thread.client_fd = sockHoster;
    thread.eventQueue = hostEventQueue;
    playersThread.push_back(thread);
}

Room::~Room() {
}

bool Room::addPlayer(int playerFd, std::shared_ptr<EventQueue> eq) {
    std::unique_lock<std::mutex> lock(mtx);

    if (matchStarted || (int)playerFds.size() >= maxPlayers) {
        return false;
    }
    ThreadGameData thread;
    eventQueue = std::make_shared<EventQueue>(); //event queue different pour chaque client?
    thread.client_fd = playerFd;
    thread.eventQueue = eq;
    playersThread.push_back(thread);
    playerFds.push_back(playerFd);
    std::cout << "Player joined room: fd=" << playerFd 
              << " (now " << playerFds.size() << " players)" << std::endl;
    cv.notify_all();
    return true;
}

void Room::removePlayer(int playerFd) {
    std::unique_lock<std::mutex> lock(mtx);

    playerFds.erase(
        std::remove(playerFds.begin(), playerFds.end(), playerFd),
        playerFds.end()
    );

    playersThread.erase(
        std::remove_if(playersThread.begin(), playersThread.end(),
            [&](const ThreadGameData& t) { return t.client_fd == playerFd; }),
        playersThread.end()
    );
    cv.notify_all();
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
            broadcastGameStart();
            matchStarted = true;
            launchGame();
            cv.notify_all();
        } else {
            std::cout << "Cannot force start, not enough players.\n";
        }
    }
}

bool Room::checkGameReady(){
    if ((int)playerFds.size() <= maxPlayers and (int)playerFds.size() >= minPlayers ){ return true; }
    return false;
}

std::shared_ptr<Game> Room::launchGame() {
    std::cout << "Launching game with " << playerFds.size() << " players\n";
    matchStarted = true;
    int numPlayers = (int)playerFds.size();
    gamePtr = std::make_shared<Game>(std::vector<ThreadGameData>(playersThread), int(numPlayers), gameMode);
    return gamePtr;
    cout<<"Created gamePtr"<<endl;
}

void Room::broadcastGameStart() {
    cout<<"Gamestrartmsg  successfully"<<endl;
    int type = MessageType::T_GameStart;
    int size = sizeof(int);
    for (int fd : playerFds) {
        sendAll(fd, &type,  sizeof(type));
        sendAll(fd, &size,  sizeof(size));
    }
    cout<<"Gamestrartmsg sent successfully"<<endl;
}


void Room::broadcastDeleteRoom() {
    int type = MessageType::T_Exit;
    int size = sizeof(int);
    for (int fd : playerFds) {
        sendAll(fd, &type,  sizeof(type));
        sendAll(fd, &size,  sizeof(size));
    }
}
// Room::void deleteRoom(){}
