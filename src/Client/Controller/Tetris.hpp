#include "ServerFunctions.hpp"
#include "InterFaceTerminal.hpp"
#include <sys/types.h>      //socket
#include <sys/socket.h>     //socket management
#include <unistd.h>         //socket
#include <string>           
#include <iostream>         //io management
#include <errno.h>          //errror management
#include <netinet/in.h>     //ip management
#include <arpa/inet.h>
#include <cstring>      // Pour memset
#include "Communication.hpp"    //sendAll & recvAll
#include "PlayerFunctions.hpp" //Functions for main
#include <csignal>      //sigint
#include <cstdlib>

#include "GuiLauncher.hpp"
#include "MenuController.hpp"

using namespace std;

extern atomic<bool> keepRunning; // to stop the program (if ctrl C for example)

class Tetris 
{
    private:
        int socketServer;
        bool isInGame;
        string username;
    public:
        Tetris();
        ~Tetris();
        
        void exitTetris();
        int createSocket();
        int runTetris(int argc, char **argv);

        int authentification(std::vector<std::string> &dataConnexion, std::shared_ptr<Player::ThreadData> &threadData, int sock, pthread_t &threadID, bool &retFlag);
};