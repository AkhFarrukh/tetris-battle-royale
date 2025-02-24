#include <sys/types.h>      //socket
#include <sys/socket.h>     //socket management
#include <unistd.h>         //socket
#include <string>           
#include <iostream>         //io management
#include <errno.h>          //errror management
#include <netinet/in.h>     //ip management
#include <arpa/inet.h>
#include <cstring>      // Pour memset

using namespace std;
const int domain = AF_INET;
const int type = SOCK_STREAM;
const int protocol = 0;
const int MAX_PENDING_CONNECTION = 10;





int main () {
    /**************************************************************
    *LAUNCH THE SERVER
    **************************************************************/
    cout << "Launching server..." << endl;

    //Create socket and return fd val
    int sock_fd = socket(domain, type, protocol);
    if (sock_fd == -1) {
        perror("Couldn't init socket : ");
        return 1;
    }

    //Def sockaddr_in
    struct sockaddr_in address;
    address.sin_family = domain;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;
    size_t addressLen = sizeof(address);

    //Bind socket to addr & port
    if (bind (sock_fd, (struct sockaddr *)&address, addressLen) <= -1){
        perror("Couldn't bind socket to address : ");
        return 1;
    }

    //Wait for a client
    if (listen (sock_fd, MAX_PENDING_CONNECTION) <= -1){
        perror("Error while listening : ");
        return 1;
    }

    /**************************************************************
    *HANDLE THE CLIENT
    **************************************************************/
    // Création du socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Erreur lors de la création du socket\n";
        return 1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Initialisation à zéro

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // Conversion de l'adresse IP en format binaire
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Adresse invalide ou non supportée\n";
        close(sock);
        return 1;
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Échec de la connexion\n";
        close(sock);
        return 1;
    }

    std::cout << "Connecté au serveur !\n";

    close(sock); // Fermer le socket après utilisation
    return 0;
}
