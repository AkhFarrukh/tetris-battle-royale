#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <iostream>             // io management
#include <sys/socket.h>         // socket management
#include <unistd.h>


bool recvAll(int sock, void* buffer, size_t length);
bool sendAll(int sock, const void* buffer, size_t length) ;

struct Message{

};


#endif