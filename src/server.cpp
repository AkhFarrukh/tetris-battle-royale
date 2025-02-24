#include <sys/types.h>      //socket
#include <sys/socket.h>     //socket management
#include <unistd.h>         //socket
#include <string>           
#include <iostream>         //io management
#include <errno.h>          //errror management
#include <netinet/in.h>     //ip management

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

}
