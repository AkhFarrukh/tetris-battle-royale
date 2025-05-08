#ifndef SERVERDATA_HPP
#define SERVERDATA_HPP


#include <string>
#include <memory>

class Server;
class EventQueue;

// All data of clients and their sessions
struct ClientSession{
    int client_fd;
    int userId;
    std::string username;
    std::string password;
    int inRoom = -1;
    bool isSpectating;

    ~ClientSession() noexcept __attribute__((noinline)) = default;
};
/**
* @brief This struct is used in grouping data to be passed into
* thread routine as only one param of type "void*" is allowed.
* It has a client session data (as usual) and a pointer to a server object.
*
* We need a server pointer because a static variable can be called
* without needing to initialise an object first so it can't access any
* specific instance of class it doesn't know exactly where the server is
* or which server to manipulate. Therefore we give it a server's pointer.
*/
struct ThreadData {
    ClientSession client;
    Server* serverInstance;
};
/**
 * @brief Struct containing the data that the thread where the game is
 * runnning needs.
 *
 * client_fd to send the gameState.
 * gameMode to init the game.
 * eventQueue so the player thread can put player input inside and game thread will treat them.
 *
 */
struct ThreadGameData {
    int client_fd;
    int gameMode;
    std::shared_ptr<EventQueue> eventQueue;
};


#endif //SERVERDATA_HPP
