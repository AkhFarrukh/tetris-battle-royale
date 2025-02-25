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
        //return 1;
        //TODO remove comment
    }

    std::cout << "Connecté au serveur !\n";

    /**********************************************************************************
     * START GAME
     *********************************************************************************/

    InterfaceTerminal terminal;
    terminal.start(); 

    close(sock); // Fermer le socket après utilisation    
    return 0;
}

//add a loop so that if i press escape in game i come back to the menu, but also when i end a game
