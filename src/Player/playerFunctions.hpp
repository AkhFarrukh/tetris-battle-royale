#ifndef PLAYER_FUNCTIONS_HPP
#define PLAYER_FUNCTIONS_HPP

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
#include "communication.hpp"    //Struct & fonctions for com w/ serv
#include "game.hpp"             //Struct PlayerState
#include "playerData.hpp"

using namespace std;

class Player {
    public:
        struct ThreadData {
            int socket;
            bool userInGame = true;
        };

        static void* threadRoutineView (void* arg); //Routine qui recoit la grid et l'affiche
        static int sendInput (std::shared_ptr<Player::ThreadData> threadData); //Envoie les inputs au server
};
     
#endif