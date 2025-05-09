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
#include "Communication.hpp"    //Struct & fonctions for com w/ serv
#include "Game.hpp"             //Struct PlayerState
#include "PlayerData.hpp"
#include <algorithm> 
#include <atomic>
#include "InterfaceTerminal.hpp"
#include "RenderRooms.hpp"
#include "Input.hpp"
#include <fcntl.h>
#include "Global.hpp"

using namespace std;
//pthread_mutex_t mutexPlayer = PTHREAD_MUTEX_INITIALIZER;
class Player {
    public:
    
        struct ThreadData {
            int socket;
            int servSocket;
	        mutex mux;
	        mutex displayMut;
	        atomic<bool> userInGame = false;
            atomic<bool> userInLobby = false;
            atomic<bool> isSpectator = false;

            bool isGui = false;
            void* qtParent = nullptr;
            std::vector<NetPlayerData> playersData;

            RoomInfo roomInfo;
            bool roomInfoUpdated = false;
            bool isOwner = false;

        

            //for the Royal competition mode to track the state of menu that sends maluses
            atomic<bool> targetPlayerMenu = false; //false if player is on malus menu, true if player is on target player menu

            atomic<Modes> gameMode; // to know if player is playing royal competition to use special display function

            atomic<int> clinetPlayerID = -1; // to know the player id of the client player in royal mode
        };

        static void* threadRoutineView (void* arg); //Routine qui recoit la grid et l'affiche
        static int sendInput (std::shared_ptr<Player::ThreadData> threadData); //Envoie les inputs au server
        static bool handleStandardInput(std::shared_ptr<Player::ThreadData> threadData, KeyboardAction action);

        static bool handlePlayerDataEndgameMessage(int socket, ThreadData *threadData, int messageSize, int messageType,
                                            RenderEngineNcurses &render);

        static bool receivePlayersData(int socket, int messageSize, int messageType, std::vector<NetPlayerData> &playersData);

        static bool roomJoinHandler(int socket, ThreadData *threadData, int messageSize);

        static bool handleRoomsSelection(int socket, ThreadData *threadData, std::vector<RoomInfo> &roomsInfo,
                                         RenderRooms &renderR,
                                         Modes mode);

        static bool handleJoinRoom(int socket, ThreadData *threadData, std::vector<RoomInfo> &roomsInfo, int index);

        static bool sendMessageToMenu(int socket, Modes mode);


        static bool roomJoinInfoHandler(int socket, ThreadData *threadData, int messageSize, int messageType);


        static bool setupRoomSession(int socket, ThreadData *threadData, int messageSize, int &flags);

        static bool processRoomInteractions(int socket, ThreadData *threadData, int messageType, int &flags);

        static bool handleRoomUserActions(int socket, ThreadData *threadData, int messageType);

        static bool handleRoomSpectatorActions(int socket, ThreadData *threadData, int action);

        static bool handleRoomPlayerActions(int socket, ThreadData *threadData, int action, int messageType);

        static bool checkForServerMessages(int socket, ThreadData *threadData);

        static bool sendMessage(int socket, int messageType);

        template<class T>
        static bool sendMessageWData(int socket, int messageType, const T &messageData);

        static bool sendExitMessage(shared_ptr<ThreadData> threadData);

        static bool sendMalBonMessage(int &chosenMalus, int &chosenPlayer, int socket);

        static bool handleMalBonMessageSending(shared_ptr<ThreadData> threadData, int &numberKey, int &chosenPlayer,
                                               int &chosenMalus);

        static int keyboardActionToInt(KeyboardAction action);
        static void affichageMessage(Message& message);
        static MalusBonus intToMalusBonus(int malus);
};
     
#endif
