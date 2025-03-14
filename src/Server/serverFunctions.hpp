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
#include "communication.hpp"
#include "game.hpp"
#include "playerData.hpp"
#include "ServerData.hpp"
#include "room.hpp"
#include <algorithm>
#include <csignal>


class Server{
    private:
        std::shared_ptr<std::vector<std::shared_ptr<Room>>> roomsList;
        std::shared_ptr<Room> myRoom;
        std::vector<ClientSession> clients;
        

        bool lockThread;
        std::mutex mtx;
        std::condition_variable cv;


    public:
        UserDatabaseManager udbm;
        int handleClient(int server_fd, struct sockaddr_in& address, size_t addressLen); 
        void broadcastMessage(const std::string& message, int sender_fd);
        void broadcastMessage(const char* buffer, const std::string& sender_username);
        static void* threadRoutineClient(void* arg);
        std::vector<ClientSession> getClients(){return clients;}
};

#endif  // SERVERFUNCTIONS_HPP

