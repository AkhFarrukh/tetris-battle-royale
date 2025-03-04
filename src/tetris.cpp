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

void* readServer(void* socket){
    int sock = *(int*)socket;
    while(true){
    char buffer[1024] = {0};
    int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
          std::cout << "Serveur déconnecté : " << sock << std::endl;
          close(sock);}
    else{
          buffer[bytesReceived] = '\0';
          size_t friendNameLength = strlen(buffer);
          std::string friendName(buffer);
          std::string message(buffer+friendNameLength+1);
          std::cout<<friendName<<endl;
          std::cout<<message<<endl;
         }
    }
}


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
    pthread_t threadLecture;
    if (pthread_create(&threadLecture, nullptr, readServer, &sock)!=0) {
            perror("error creation thread");
            return 1;
        }
    std::cout << "Connecté au serveur !\n";
    InterfaceTerminal terminal;

    bool playing = true;
    while(playing){
        vector<std::string> dataConnexion = terminal.handleConnexionOption();
        if(dataConnexion.size()==2){
            if (send(sock, dataConnexion[0].c_str(), dataConnexion[0].length(), 0) <= 0 ||
            send(sock, dataConnexion[1].c_str(), dataConnexion[1].length(), 0) <= 0) {
            perror("Erreur lors de l'envoie des credentials");
            close(sock);
            return 0;
        }
    }
        else{
           std::cout << "Echec de la connexion au compte.\n";
           close(sock); // Fermer le socket après utilisation
           return 0;
    }
        terminal.setSock(sock);
        int selectedMode = terminal.start();
        if (selectedMode == -1){
            //Quit game
            playing = false;
        }
        else{

        }
    }

    /**********************************************************************************
     * START GAME
     *********************************************************************************/



    close(sock); // Fermer le socket après utilisation
    return 0;
}
//add a loop so that if i press escape in game i come back to the menu, but also when i end a game
