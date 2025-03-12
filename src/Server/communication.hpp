#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <iostream>             // io management
#include <sys/socket.h>         // socket management
#include <unistd.h>
#include "keyboardAction.hpp"

using namespace std;

bool recvAll(int sock, void* buffer, size_t length);
bool sendAll(int sock, const void* buffer, size_t length);

enum Modes{
    Endless,
    Duel,
    Royal,
    Classic
};

struct Message{
    string destinataire;
    string message;
};

struct InputMessage{
    KeyboardAction input;
};

enum MenuActions{
    A_Play,
    A_CreateRoom,
    A_JoinRoom,
    A_Chat,
};

struct Menu{
    MenuActions action;
    Modes mode;
};



enum MessageType {
    T_Message,
    T_Input,
    T_Menu,
    T_PlayerData
};

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


#endif