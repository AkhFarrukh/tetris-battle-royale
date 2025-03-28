#include "serverFunctions.hpp"
#include "InterFaceTerminal.hpp"
#include "MenuGameModeController.hpp"
#include <sys/types.h>      //socket
#include <sys/socket.h>     //socket management
#include <unistd.h>         //socket
#include <string>           
#include <iostream>         //io management
#include <errno.h>          //errror management
#include <netinet/in.h>     //ip management
#include <arpa/inet.h>
#include <cstring>      // Pour memset
#include "communication.hpp"    //sendAll & recvAll
#include "playerFunctions.hpp" //Functions for main
#include <csignal>      //sigint
#include <cstdlib>

#include "Gui/gui_launcher.hpp"

using namespace std;


std::string username = "";
int socket_serveur = -1;
atomic<bool> isInGame = false;

void handle_sigint(int sig) {
   flag = true;
   if(socket_serveur != -1){
     if(isInGame){
         int type = MessageType::T_Input;
         int size = sizeof(InputMessage);
         InputMessage inputMessage;
         inputMessage.input = EXIT_KEY;
         sendAll(socket_serveur, &type, sizeof(int));
         sendAll(socket_serveur, &size, sizeof(int));
         sendAll(socket_serveur, &inputMessage, sizeof(InputMessage));
     }
    int exitType = MessageType::T_ForcedExit;
    int size_ = sizeof(int);
    int bytesSentType = sendAll(socket_serveur, &exitType, sizeof(int));
    int bytesSentSize = sendAll(socket_serveur, &size_, sizeof(int));}
}

int createSocket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Erreur lors de la création du socket : ");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Adresse invalide : ");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Échec de la connexion : ");
        close(sock);
        return -1;
    }
    return sock;
}



int main(int argc, char** argv) {
    signal(SIGINT, handle_sigint);
    // ==== 1. Connexion au serveur ====
    int sock = createSocket();
    if (sock < 0) return 1;

    socket_serveur = sock;


    pthread_t threadID;
    std::shared_ptr<Player::ThreadData> threadData = std::make_shared<Player::ThreadData>();
    //Game loop
    if (argc > 1 && std::string(argv[1]) == "--gui") {
        // threadData->socket = sock;
        // threadData->userInGame = false;
        // threadData->userInLobby = false;
        // if (pthread_create(&threadID, nullptr, Player::threadRoutineView, threadData.get()) < 0) {
        //     perror("Error on thread creation : ");
        //     close(sock);
        //     return 1;
        // }
    
        return runGuiApp(argc, argv);

    } else {
        bool playing = false;
        InterfaceTerminal terminal;
        socket_serveur = sock;
        terminal.setSock(socket_serveur);
        //Identification auprès de la db
        vector<std::string> dataConnexion = terminal.handleConnexionOption();
        if(dataConnexion.size()==2){
            username = dataConnexion[0];
            playing = true;
            threadData->userInGame = false;
            threadData->socket = sock;
            threadData->userInLobby = false;
            if (pthread_create(&threadID, nullptr, Player::threadRoutineView, threadData.get()) < 0) {
                perror("Error on thread creation : ");
                close(sock); // Fermer le socket après utilisation
                return 0;
            }
        }else{
            std::cout << "Echec de la connexion au compte.\n";
            nocbreak();
            keypad(stdscr, false);
            curs_set(1);
            nodelay(stdscr, false);
            system("stty echo"); // Pour que le client réaffiche ce qu'on ecrit
            close(sock); // Fermer le socket après utilisation
            std::string command = "clear";
            int result = system(command.c_str());
            return 0;
        }
        while(playing && !flag){
            //Getting the selected menu in terminal
            int selectedMode = terminal.start();
            if (selectedMode == -1){
                //Quit game
                int exitType = MessageType::T_ForcedExit;
                int size_ = sizeof(int);
                int bytesSentType = sendAll(socket_serveur, &exitType, sizeof(int));
                int bytesSentSize = sendAll(socket_serveur, &size_, sizeof(int));
                playing = false;
            }
            else{
                /*
                *SHOULD BE ELSE IF AND CHANGE DEPENDING ON MENU CLICKED
                */
                //Ask to start a game
                int type = MessageType::T_Menu;
                int size = sizeof(Menu);
                //Defining the struct we will send  
                Menu menu;
                menu.action = MenuActions::A_Play;
                if(selectedMode == 0){
                    menu.mode = Modes::Endless_;
                    //napms(500);
                }
                else if(selectedMode == 10){
                    menu.mode = Modes::Classic_;
                    menu.action = MenuActions::A_CreateRoom;
                }
                else if(selectedMode == 11){
                    menu.mode = Modes::Classic_;
                    menu.action = MenuActions::A_JoinRoom;
                }
                else if(selectedMode == 20){
                    menu.mode = Modes::Duel_;
                    menu.action = MenuActions::A_CreateRoom;
                }
                else if(selectedMode == 21){
                    menu.mode = Modes::Duel_;
                    menu.action = MenuActions::A_JoinRoom;
                }
                else if(selectedMode == 30){
                    menu.mode = Modes::Royal_;
                    menu.action = MenuActions::A_CreateRoom;
                }
                else if(selectedMode == 31){
                    menu.mode = Modes::Royal_;
                    menu.action = MenuActions::A_JoinRoom;
                }
                //Sending info to server
                if(sendAll(sock, &type, sizeof(int)) == false){
                    perror("Error sending menu info : ");
                    return 1;
                }
                if(sendAll(sock, &size, sizeof(int)) == false){
                    perror("Error sending menu info : ");
                    return 1;
                }
                if(sendAll(sock, &menu, sizeof(Menu)) == false){
                    perror("Error sending menu info : ");
                    return 1;
                }
                threadData->mux.lock();
                threadData->userInLobby = true;
                threadData->gameMode = menu.mode;
                threadData->mux.unlock();

                Player::sendInput(threadData);
                threadData->mux.lock();
                threadData->userInGame = false;
                threadData->userInLobby = false;
                threadData->mux.unlock();
                napms(600);
                if(menu.action == MenuActions::A_CreateRoom){
                    int sendType = MessageType::T_RoomDeleted;
                    int sendSize = sizeof(int);
                    if(!sendAll(sock, &sendType, sizeof(sendType))){flag = true; break;}
                    if(!sendAll(sock, &sendSize, sizeof(sendSize))){flag = true; break;}
                }

            }
        }

        flag = true;//thread principal a terminé et va demander à threadLecture de se terminer.
        shutdown(sock, SHUT_RD);
        pthread_join(threadID, nullptr);

        ///**********************************************************************************
        // * START GAME
        // *********************************************************************************/

        //on remet ces fct dans leurs etats initiaux (utile notamment si on a fermé le programme avec un SIGINT)
        nocbreak();
        keypad(stdscr, false);
        curs_set(1);
        nodelay(stdscr, false);
        system("stty echo"); // Pour que le client réaffiche ce qu'on ecrit
        close(sock); // Fermer le socket après utilisation
        std::string command = "clear";
        int result = system(command.c_str());
        return 0;

    }
}

/**
 * @brief Manière d'envoyer un message au serv
 * Def a type   : int type = MessageType::Input
 * Def the size : int size = sizeof(struct Input)
 * 
 * send (sock, &type, sizeof(int), 0);
 * send (sock, &size, sizeof(int), 0);
 * send (sock, &messageOfTypeInput, sizeof(Input), 0)
 * 
 * NB : Le faire avec sendAll mais le principe reste le même
 */




