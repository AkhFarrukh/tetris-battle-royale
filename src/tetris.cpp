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

using namespace std;


std::string username = "";
int socket_serveur = -1;

void handle_sigint(int sig) {
   flag = true;
   if(socket_serveur != -1){
    int exitType = MessageType::T_ForcedExit;
    int size_ = sizeof(int);
    int bytesSentType = sendAll(socket_serveur, &exitType, sizeof(int));
    int bytesSentSize = sendAll(socket_serveur, &size_, sizeof(int));}
}


void affichageMessage(const std::string& sender, const std::string& message){
    nodelay(stdscr, false);
    ////
    interfaceMutex.lock();
    refresh();
    curs_set(0);
    int currentLine = LINES-1;
    while(mvinch(currentLine-1, 0) == ' ' || mvinch(currentLine-1, 0) == ERR){ //tant que ligne est vide
        currentLine--;
        }
    move(currentLine, 0);
    printw("%s : %s\n", sender.c_str(), message.c_str());
    if (LINES-currentLine<5) {
            move(LINES-1, 0);
            clrtoeol();
            scrollok(stdscr, true);
            scroll(stdscr);
            scrollok(stdscr, false);
            move(3, 0);clrtoeol();
            move(2, 0);clrtoeol();
            move(1, 0);clrtoeol();
            move(0, 0);clrtoeol();
            printw("Press ENTER to send the message\nPress 'ENTER' with an empty entry to quit\n");
            printw("\nDiscussion with %s : \n", sender.c_str());
        }
        move(LINES- 1, 0);
        clrtoeol();
        printw("Enter your message : ");
        refresh();
        curs_set(1);
        interfaceMutex.unlock();
}


void* readServer(void* socket){
    int sock = *(int*)socket;
    while(!flag){
    if(username == "")break;
    char buffer[1024] = {0};
    int bytesReceived = recv(sock, buffer, sizeof(buffer)-1, 0);
    if (bytesReceived <= 0) {
          flag = true;
          return nullptr;
    }
    else{
          buffer[bytesReceived] = '\0';
          size_t friendNameLength = strlen(buffer);
          std::string friendName(buffer);
          std::string message(buffer+friendNameLength+1, bytesReceived - friendNameLength-1);
          affichageMessage(friendName, message);
         }
    }
    return nullptr;
}



int main() {
    signal(SIGINT, handle_sigint);
    /**********************************************************************************
     *SETUP CONNECTION TO SERVER
     *********************************************************************************/
    // Création du socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Erreur lors de la création du socket : ");
        return 1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Initialisation à zéro

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // Conversion de l'adresse IP en format binaire
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Adresse invalide ou non supportée : ");
        close(sock);
        return 1;
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Échec de la connexion : ");
        close(sock);
        return 1;
    }
    pthread_t threadLecture;
    if (pthread_create(&threadLecture, nullptr, readServer, &sock)!=0) {
            perror("error creation thread");
            return -1;
        }

    //std::cout << "Connecté au serveur !\n";

    //Game loop
    bool playing = true;
    while(playing){
        InterfaceTerminal terminal;
        socket_serveur = sock;
        terminal.setSock(socket_serveur);
        //Identification auprès de la db
        vector<std::string> dataConnexion = terminal.handleConnexionOption();
        if(dataConnexion.size()==2){
            username = dataConnexion[0];}
        else{
           std::cout << "Echec de la connexion au compte.\n";
           playing = false;
           break;
        }
        /**************************************CRASH AFTER******************************************************************* */
        //Getting the selected menu in terminal
        int selectedMode = terminal.start();
        if (selectedMode == -1){
            //Quit game
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
                //menu.mode = InterfaceTerminal::modeNames[selectedMode];
                menu.mode = Modes::Endless;
                napms(500);

            }
            else if(selectedMode == 10){
                menu.mode = Modes::Classic;
                menu.action = MenuActions::A_CreateRoom;
            }
            else if(selectedMode == 11){
                menu.mode = Modes::Classic;
                menu.action = MenuActions::A_JoinRoom;
            }
            else if(selectedMode == 20){
                menu.mode = Modes::Duel;
                menu.action = MenuActions::A_CreateRoom;
            }
            else if(selectedMode == 21){
                menu.mode = Modes::Duel;
                menu.action = MenuActions::A_JoinRoom;
            }
            else if(selectedMode == 30){
                menu.mode = Modes::Royal;
                menu.action = MenuActions::A_CreateRoom;
            }
            else if(selectedMode == 31){
                menu.mode = Modes::Royal;
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

            /*
            *LOOP RECEIVING GRID AND SHOWING IT, ON ANOTHER THREAD
            */
            pthread_t threadID;
            std::shared_ptr<Player::ThreadData> threadData = std::make_shared<Player::ThreadData>();
            threadData->userInGame = true;
	        threadData->socket = sock;

            if (pthread_create(&threadID, nullptr, Player::threadRoutineView, threadData.get()) < 0) {
                perror("Error on thread creation : ");
                playing = false;
            }

            /*
            *LOOP GETTING INPUTS AND SENDING IT TO SERV
            */
            Player::sendInput(threadData);
            napms(500);
            pthread_join(threadID, nullptr);
        }
    }

    flag = true;//thread principal a terminé et va demander à threadLecture de se terminer.
    shutdown(sock, SHUT_RD);
    pthread_join(threadLecture, nullptr);

    ///**********************************************************************************
    // * START GAME
    // *********************************************************************************/

    //on remet ces fct dans leurs etats initiaux (utile notamment si on a fermé le programme avec un SIGINT)
    nocbreak();
    keypad(stdscr, false);
    curs_set(1);
    nodelay(stdscr, false);
    close(sock); // Fermer le socket après utilisation
    return 0;
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
