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

using namespace std;

int main() {
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

    std::cout << "Connecté au serveur !\n";

    //Game loop
    bool playing = true;
    while(playing){
        InterfaceTerminal terminal;
        int selectedMode = terminal.start(); 
        if (selectedMode == -1){
            //Quit game
            playing = false;
        }
        else{
            //Ask to start a game
            int type = MessageType::T_Menu;
            int size = sizeof(type);

            //Defining the struct we will send
            Menu menu;
            menu.action = MenuActions::A_Play;
            //menu.mode = InterfaceTerminal::modeNames[selectedMode];
            menu.mode = "Endless Mode";

            //Sending info to server
            send (sock, &type, sizeof(int), 0);
            send (sock, &size, sizeof(int), 0);
            sendAll(sock, &menu, sizeof(Menu));
            
            /*
            *NEED TO IMPLEMENT LOOP GETTING INPUTS AND SENDING IT TO SERV
            */

            /*
            *NEED TO IMPLEMENT LOOP RECEIVING GRID AND SHOWING IT, ON ANOTHER THREAD
            */

        }
    }
    close(sock); // Fermer le socket après utilisation
    return 0;
}
//add a loop so that if i press escape in game i come back to the menu, but also when i end a game


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