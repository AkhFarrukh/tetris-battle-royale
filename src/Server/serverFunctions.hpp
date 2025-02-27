#ifndef SERVER_FUNCTIONS_HPP
#define SERVER_FUNCTIONS_HPP

#include <sys/types.h>      //socket
#include <sys/socket.h>     //socket management
#include <unistd.h>         //socket
#include <string>           
#include <iostream>         //io management
#include <errno.h>          //errror management
#include <netinet/in.h>     //ip management

int handleClient (int server_fd, sockaddr_in address, size_t addressLen);   //Peut prendre bcp plus d'arg si on veut, 
                                                                            //à voir ca qu'on veut qu'elle fasse exactement
                                                                            //Contient entre autres accept()

#endif  // SERVER_FUNCTIONS_HPP

