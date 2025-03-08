#include "room.hpp"
#include "game.hpp"
#include "EventQueue.hpp"
#include <iostream>
#include <thread>

Room::Room(int minPlayers, int maxPlayers)
    : minPlayers(minPlayers),
      maxPlayers(maxPlayers),
      matchStarted(false)
{
    eventQueue = std::make_shared<EventQueue>();
}

Room::~Room() {
}

bool Room::addPlayer(int playerFd) {
    std::unique_lock<std::mutex> lock(mtx);

    if (matchStarted || (int)playerFds.size() >= maxPlayers) {
        return false;
    }

    playerFds.push_back(playerFd);
    std::cout << "Player joined room: fd=" << playerFd 
              << " (now " << playerFds.size() << " players)" << std::endl;

    if ((int)playerFds.size() >= maxPlayers) {
        std::cout << "Room is full, automatically starting match..." << std::endl;
        matchStarted = true;
        launchGame();
        cv.notify_all();
    } else {
        cv.notify_all();
    }

    return true;
}

int Room::getPlayerCount() const {
    std::lock_guard<std::mutex> lock(mtx);
    return (int)playerFds.size();
}

void Room::startMatchIfPossible() {
    std::unique_lock<std::mutex> lock(mtx);
    if (!matchStarted) {
        // We can only start if we have at least minPlayers
        if ((int)playerFds.size() >= minPlayers) {
            matchStarted = true;
            std::cout << "Match started with " << playerFds.size() << " players\n";
            launchGame();
            cv.notify_all();
        } else {
            std::cout << "Not enough players yet to start (need at least " 
                      << minPlayers << ")\n";
        }
    }
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

void Room::waitForMatchToStart() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return matchStarted; });
    // Once we pass this, the match has started
}

void Room::launchGame() {
    std::cout << "Launching game with " << playerFds.size() << " players\n";
    
    int numPlayers = (int)playerFds.size();
    gamePtr = std::make_shared<Game>(eventQueue, numPlayers);

    // gamePtr->setPlayerFDs(playerFds);

    std::thread gameThread([this] {
        gamePtr->run();
    });
    gameThread.detach();

}
