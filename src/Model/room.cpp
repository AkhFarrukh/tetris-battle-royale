/*
#include "room.hpp"
#include "game.hpp"
#include "EventQueue.hpp"
#include <iostream>
#include <thread>

Room::Room(int minPlayers, int maxPlayers)
    : minPlayers(minPlayers),
      maxPlayers(maxPlayers),
      matchStarted(false)
{}

Room::~Room() {
}

bool Room::addPlayer(int playerFd) {
    std::unique_lock<std::mutex> lock(mtx);

    // Compare players.size() with maxPlayers
    if (matchStarted || (int)players.size() >= maxPlayers) {
        return false;
    }

    ThreadGameData newplayer;
    newplayer.client_fd = playerFd;
    newplayer.eventQueue = std::make_shared<EventQueue>();

    players.push_back(newplayer);

    std::cout << "Player joined room: fd=" << playerFd 
              << " (now " << players.size() << " players)" << std::endl;

    if ((int)players.size() >= maxPlayers) {
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
        if ((int)players.size() >= minPlayers) {
            matchStarted = true;
            std::cout << "Match started with " << players.size() << " players\n";
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
    std::cout << "Launching game with " << players.size() << " players\n";

    int numPlayers = (int)players.size();
    gamePtr = std::make_shared<Game>(players, numPlayers, "Endless");

    std::thread gameThread([this] {
        gamePtr->run();
    });
    gameThread.detach();
}

*/