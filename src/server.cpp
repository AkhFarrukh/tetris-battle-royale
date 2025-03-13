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
UserDatabaseManager userdata;
Database basedata("tetris.db");

void handle_sigint(int sig) {
    std::vector<ClientSession> clients;// = server.getClients(); NEED IMPLEMENTATION
    for(const auto& client:clients){
    int user_id = basedata.getUserId(client.username.c_str());
    while(userdata.isLoggedIn(client.username.c_str())){
        userdata.logOut(user_id);} //on marque la personne comme deconnecté
    basedata.setNotInConversation(user_id); //si la personne etait sur une conv cela permet qu'elle n'y soit plus marqué
                                                                                    //comme telle
    while(basedata.isUserInSession(user_id)){
        basedata.deleteSession(user_id);}}
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

    //Def sockaddr_in
    struct sockaddr_in address;
    address.sin_family = domain;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;
    size_t addressLen = sizeof(address);

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
    Server server;
    auto rooms = std::make_shared<std::vector<std::shared_ptr<Room>>>();

    if (server.handleClient(server_fd, address, addressLen, rooms) <= -1){
        perror("Error while handling client : ");
        close(server_fd);
        return 1;
    }

    close(server_fd);
    return 0;
}
