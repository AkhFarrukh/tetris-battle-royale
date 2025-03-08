#ifndef ROOM_HPP
#define ROOM_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>

// Forward declarations
class Game;
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
    Room(int minPlayers = 3, int maxPlayers = 9);

    ~Room();

    /**
     * @brief Adds a new player (by FD or ID).
     * @returns true if successfully added, false if room is full.
     */
    bool addPlayer(int playerFd);

    /**
     * @brief Tells the room to try launching the game if conditions are met 
     * (i.e., #players >= minPlayers).
     */
    void startMatchIfPossible();

    /**
     * @brief Force start if we have at least minPlayers. 
     * (In case you want a "start game" button.)
     */
    void forceStart();

    /**
     * @brief Check how many players have joined so far.
     */
    int getPlayerCount() const;

    /**
     * @brief The main logic that blocks until the match is started.
     * Or you can rely on some external call to startMatchIfPossible().
     */
    void waitForMatchToStart();

private:
    /**
     * @brief Actually create the Game object with the current players,
     * and run it in a separate thread or in the same thread—depending on your design.
     */
    void launchGame();

private:
    int minPlayers;
    int maxPlayers;

    std::vector<int> playerFds;  // store FDs or IDs for each joined player
    bool matchStarted;

    // If you want concurrency around addPlayer, etc.
    mutable std::mutex mtx;
    std::condition_variable cv;

    // The game object (once started)
    std::shared_ptr<Game> gamePtr;

    // Possibly an EventQueue for the match
    std::shared_ptr<EventQueue> eventQueue;
};

#endif // ROOM_HPP
