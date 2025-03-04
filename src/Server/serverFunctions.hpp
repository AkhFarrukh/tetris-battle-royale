#ifndef SERVER_FUNCTIONS_HPP
#define SERVER_FUNCTIONS_HPP

#include <sys/select.h>
#include <sys/socket.h>         // socket management
#include <sys/types.h>          // socket
#include <unistd.h>             // socket
#include <string>           
#include <iostream>             // io management
#include <errno.h>              // errror management
#include <netinet/in.h>         // ip management
#include <arpa/inet.h>
#include <vector>
#include <cstring>
#include <mutex>                // Concurent access
#include <condition_variable>   // Thread sync
#include <pthread.h>            // Threads
#include <sqlite3.h>
#include "bcrypt.h"
#include "database.hpp"
#include "userdatabasemanager.hpp"
#include "EventQueue.hpp"


class Server{
    private:

        // All data of clients and their sessions
        struct ClientSession{
            int client_fd;
            int userId;
            std::string username;
            std::string password;
        };
        std::vector<ClientSession> clients;

        // Synchronisation management
        std::mutex mtx;
        std::condition_variable cv;
        bool lockThread;

        // Database part
        Database database{"tetris.db"};
        UserDatabaseManager udbm; // Abbreviation of User Database Manager

    public:
        int handleClient(int server_fd, struct sockaddr_in& address, size_t addressLen); 
        void broadcastMessage(const std::string& message, int sender_fd);
        static void* threadRoutineClient(void* arg);
        
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
            shared_ptr<EventQueue> eventQueue;
        };
};

#endif  // SERVER_FUNCTIONS_HPP

