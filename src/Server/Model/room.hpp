#ifndef ROOM_HPP
#define ROOM_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "ServerData.hpp"
#include "EventQueue.hpp"
#include <iostream>
#include <thread>
#include "communication.hpp"

static int globalRoomCounter = 0;
class Game; // Forward declaration

// Forward declarations
class EventQueue;

/**
 * @brief A Room object is created when a client starts a match.
 * It waits for players to join. Once it has between 3 and 9 players,
 * it can start. If it reaches 9 automatically, it starts as well.
 */
class Room {
public:
    /**
     * @param minPlayers Minimum number of players required to start (3).
     * @param maxPlayers Maximum number of players in the room (9).
     */
    Room(int sockHoster, Modes mode, std::shared_ptr<EventQueue> hostEventQueue, int maxPlayers);
    Room();

    ~Room();

    int getOwnerFd() const { return sockHoster; }
    int getRoomID() const { return roomID; }

    /**
     * @brief Adds a new player (by FD or ID).
     * @returns true if successfully added, false if room is full.
     */
    bool addPlayer(int playerFd, std::shared_ptr<EventQueue> eq);
    void removePlayer(int playerFd);

    /**
     * @brief Force start if we have at least minPlayers. 
     * (In case you want a "start game" button.)
     */
    void forceStart();

    /**
     * @brief Check how many players have joined so far.
     */
    int getPlayerCount() const;

    void broadcastGameStart();

    void broadcastDeleteRoom();

    Modes getRoomMode(){
        return gameMode;
    }

    bool checkGameReady();

    bool isStarted();

    std::shared_ptr<Game> launchGame();

    bool addSpectator(int spectatorFd);
    bool removeSpectator(int spectatorFd);
    

    // The game object (once started)
    std::shared_ptr<Game> gamePtr;

private:
    /**
     * @brief Actually create the Game object with the current players,
     * and run it in a separate thread or in the same thread—depending on your design.
     */
    

private:
    int sockHoster;
    Modes gameMode;
    int minPlayers = 3, maxPlayers = 9;
    bool matchStarted;
    int roomID;

    std::vector<ThreadGameData> playersThread;
    std::vector<int> playerFds;  // store FDs or IDs for each joined player
    std::vector<int> spectatorFds;


    // If you want concurrency around addPlayer, etc.
    mutable std::mutex mtx;
    std::condition_variable cv;

    

    // Possibly an EventQueue for the match
    std::shared_ptr<EventQueue> eventQueue;
};

#endif // ROOM_HPP
