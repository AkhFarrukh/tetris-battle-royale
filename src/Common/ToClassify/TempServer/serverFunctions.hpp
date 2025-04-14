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

class GameController;
extern atomic<bool> flagServ;

class Server{
    private:
        std::shared_ptr<std::vector<std::shared_ptr<Room>>> roomsList;
        std::shared_ptr<Room> myRoom;
        std::vector<ClientSession> clients;
        

        bool lockThread;
        std::mutex mtx;
        std::mutex clientMutex;
        std::condition_variable cv;
        bool userIsInLobby = false;

        /* Methods handling the different message received from the client */
        void handleDisconnect(Server* server, ClientSession& client, pthread_t& threadPID);
        void handleForcedExit(Server* server, ClientSession& client);
        void handleJoin(Server* server, ClientSession& client, bool& userIsOnline, int& size, shared_ptr<EventQueue> eventQueue);
        void handleLeftRoom(Server* server, ClientSession& client);
        void handleRefresh(Server* server, ClientSession& client, bool& userIsOnline, bool& owner);
        void handleForceStart(Server* server, ClientSession& client, bool& userIsOnline, pthread_t& threadGamePID);
        void handleGetRoom(Server* server, ClientSession& client, bool& userIsOnline, MenuMessage& MenuMessage);
        void handleCreateRoom(Server* server, ClientSession& client, bool& userIsOnline, MenuMessage& MenuMessage, shared_ptr<EventQueue> eventQueue);
        void handleDeleteRoom(Server* server, ClientSession& client);
        void handlePlay(Server* server, ClientSession& client, bool& userIsOnline, shared_ptr<EventQueue> eventQueue, ThreadGameData& threadGameData);
        void handleMalusBonus(ClientSession& client, bool& userIsOnline, shared_ptr<EventQueue> eventQueue);
        void handleInput(ClientSession& client, bool& userIsOnline, shared_ptr<EventQueue> eventQueue, GameController& gameController);
        void handleConnexion(Server* server, ClientSession& client, Connexion& connexionData, int& size, bool& successfulLogin, bool& flagConnexion);
        void handleRegister(Server* server, ClientSession& client, Connexion& connexionData, int& size, bool& successfulLogin, bool& flagConnexion);



    public:
        Server();
        UserDatabaseManager udbm;
        int handleClient(int server_fd, struct sockaddr_in& address, size_t addressLen);
        void broadcastMessage(Message message, bool& userIsOnline);
        static void* threadRoutineClient(void* arg);
        //std::vector<ClientSession>& getClients(){return clients;}
        static void* threadRoutineGame(void* arg);

        /* Login Methods */
        void notifySuccessfulLogin(Server* server, ClientSession& client, Connexion& connexion, string& username, 
                        bool& successfulLogin, int& typeConnexion, int& doubleSession, bool& flagConnexion);
        void notifyRefusedLogin(ClientSession& client);
        void notifyAcceptConnexion(Server* server, ClientSession& client, Connexion& connexionData, bool& successfulLogin); 
        
};

#endif  // SERVERFUNCTIONS_HPP

