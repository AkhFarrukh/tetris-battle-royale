#include <sys/types.h>      //socket
#include <sys/socket.h>     //socket management
#include <unistd.h>         //socket
#include <string>           
#include <iostream>         //io management
#include <errno.h>          //errror management
#include <netinet/in.h>     //ip management

#include "serverFunctions.hpp"
#include "room.hpp"

using namespace std;
const int domain = AF_INET;
const int type = SOCK_STREAM;
const int protocol = 0;
const int MAX_PENDING_CONNECTION = 10;

Server server;
int server_sock = -1;

void handle_sigint(int sig) {
    std::vector<int> clients = server.udbm.getUserIdSession();
    for(const auto& clientId:clients){
        printf("%d \n", clientId);
        server.udbm.logOut(clientId);//on marque la personne comme deconnecté
        server.udbm.setNotInConversation(clientId); //si la personne etait sur une conv cela permet qu'elle n'y soit plus marqué
                                                                                    //comme telle
        int sock_client = server.udbm.getSockUser(clientId);
        int exitType = MessageType::T_ForcedExit;
        int size_ = sizeof(int);
        int bytesSentType = sendAll(sock_client, &exitType, sizeof(int));
        int bytesSentSize = sendAll(sock_client, &size_, sizeof(int));
    while(server.udbm.isUserInSession(clientId)){
        server.udbm.deleteSession(clientId);}
    }
    napms(100);
    if(server_sock != -1)close(server_sock);
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}


int main () {
    signal(SIGINT, handle_sigint);
    /**************************************************************
    *LAUNCH THE SERVER
    **************************************************************/
    cout << "Launching server..." << endl;

    //Create socket and return fd val
    int server_fd = socket(domain, type, protocol);
    if (server_fd == -1) {
        perror("Couldn't init socket : ");
        return 1;
    }
    server_sock = server_fd;
    //Def sockaddr_in
    struct sockaddr_in address;
    address.sin_family = domain;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;
    socklen_t addressLen = sizeof(address);

    //Bind socket to addr & port
    if (bind (server_fd, (struct sockaddr *)&address, static_cast<socklen_t>(addressLen)) <= -1){
        perror("Couldn't bind socket to address : ");
        return 1;
    }
    //Wait for a client
    if (listen (server_fd, MAX_PENDING_CONNECTION) <= -1){
        perror("Error while listening : ");
        return 1;
    }

    /**************************************************************
    *HANDLE THE CLIENT
    **************************************************************/

    auto rooms = std::make_shared<std::vector<std::shared_ptr<Room>>>();

    if (server.handleClient(server_fd, address, addressLen) <= -1){
        perror("Error while handling client : ");
        close(server_fd);
        return 1;
    }

    close(server_fd);
    return 0;
}
