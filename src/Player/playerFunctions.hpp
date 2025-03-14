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
#include <algorithm> 
#include <atomic>
#include "InterFaceTerminal.hpp"

using namespace std;
//pthread_mutex_t mutexPlayer = PTHREAD_MUTEX_INITIALIZER;
extern atomic<bool> flag;

class Player {
    public:
        struct ThreadData {
            int socket;
	    mutex mux;
	    mutex displayMut;
	    atomic<bool> userInGame = true;

        //for the Royal competition mode to track the state of menu that sends maluses
        atomic<bool> targetPlayerMenu = false; //false if player is on malus menu, true if player is on target player menu
        };

        static void* threadRoutineView (void* arg); //Routine qui recoit la grid et l'affiche
        static int sendInput (std::shared_ptr<Player::ThreadData> threadData); //Envoie les inputs au server
};
     
#endif
