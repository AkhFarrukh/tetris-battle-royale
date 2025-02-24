#include <sys/types.h>      //socket
#include <sys/socket.h>     //socket
#include <unistd.h>         //socket
#include <string>           
#include <iostream>
#include <errno.h>

using namespace std;
const int domain = AF_INET;
const int type = SOCK_STREAM;
const int protocol = 0;





int main () {
    cout << "Launching server..." << endl;
    /* Create socket and return fd val */
    int sock_fd = socket(domain, type, protocol);
    if (sock_fd == -1) {
        perror("Couldn't init socket : ");
        return 1;
    }

    /* Bind socket to addr & port */
    
}
