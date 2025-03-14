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

void handle_sigint(int sig) {
   flag = true;
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
        terminal.setSock(sock);
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
            *SHOULD BE ELSE IF AND CHANGE DEPENDING ON SELECTEDMODE
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
            else if(selectedMode == 1){
                //menu.mode = "Classic Mode";
            }
            else if(selectedMode == 2){
                //menu.mode = "Duel Mode";
            }
            else if(selectedMode == 3){
                //menu.mode = "Royal Competition Mode";
            }
            //printf("sock 0 = %d\n", sock);
            //Sending info to server
            if(sendAll(sock, &type, sizeof(int)) == false){
                perror("Error while sending info : ");
                return 1;
            }
            if(sendAll(sock, &size, sizeof(int)) == false){
                perror("Error while sending info : ");
                return 1;
            }
            if(sendAll(sock, &menu, sizeof(Menu)) == false){
                perror("Error while sending info : ");
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
            //printf("player loop in tetrip.cpp\n");
            /*
            *LOOP GETTING INPUTS AND SENDING IT TO SERV
            */
            //void* ret;
            /* TEMPORARY, TO TEST WITHOUT MANAGING INPUT HANDLING */
            // pthread_join(threadID, &ret);
            Player::sendInput(threadData);
            napms(500);
        }
    }
    /* COMING FROM CHAT, BUT BAD IDEA IMO*/
    //flag = true;//thread principal a terminé et va demander à threadLecture de se terminer.
    //shutdown(sock, SHUT_RD);
    //pthread_join(threadLecture, nullptr);
    ///**********************************************************************************
    // * START GAME
    // *********************************************************************************/
    //
    //
    //if (username != ""){ //si la personne s'est connecté
    //	int user_id = db.getUserId(username.c_str());
    //    while(udm.isLoggedIn(username.c_str())){
    //	udm.logOut(user_id);} //on marque la personne comme deconnecté
    //    db.setNotInConversation(user_id); //si la personne etait sur une conv cela permet qu'elle n'y soit plus marqué
    //                                                                                        //comme telle
	//	while(db.isUserInSession(user_id)){
  	//		db.deleteSession(user_id);}
    //}
    ////on remet ces fct dans leurs etats initiaux (utile notamment si on a fermé le programme avec un SIGINT)
    //nocbreak();
    //keypad(stdscr, false);
    //curs_set(1);
    //nodelay(stdscr, false);
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
