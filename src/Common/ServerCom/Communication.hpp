#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <iostream>             // io management
#include <sys/socket.h>         // socket management
#include <unistd.h>
#include "KeyboardAction.hpp"
#include "ControllerEnum.hpp"

using namespace std;

bool recvAll(int sock, void* buffer, size_t length);
bool sendAll(int sock, const void* buffer, size_t length);

enum Modes{
    NONE_,
    Endless_,
    Duel_,
    Royal_,
    Classic_
};

struct Message{
    char destinataire[32];
    char expediteur[32];
    char message[1024];
};

struct InputMessage{
    KeyboardAction input;
};
struct MalBonus{
    MalusBonus malusB;
    int target;
};

enum MenuActions{
    A_Play,
    A_CreateRoom,
    A_JoinRoom,
    A_Chat,
    A_ForceStart
};

struct MenuMessage{
    MenuActions action;
    Modes mode;
    int maxPlayers;
};


struct Connexion{
    char username[100];
    char password[100];
};


struct FriendsList {
    int   count;
    char  names[50][32];
};

struct InvitedFriend {
    char friendUsername[32];
};

struct RoomInvitation {
    char inviterUsername[32];
    int  roomId;
    int  mode;
};

enum MessageType {
    T_Message,
    T_Input,
    T_MalusBonus,
    T_Menu,
    T_PlayerData,
    T_EndGame,
    T_Exit,
    T_Connexion,
    T_Register,
    T_AcceptConnexion,
    T_RefuseConnexion,
    T_Data,
    T_ForcedExit,
    T_RoomInfo,
    T_RoomJoinInfo,
    T_RoomsInfo,
    T_Refresh,
    T_RoomJoinRefresh,
    T_Join,
    T_RoomDeleted,
    T_GameStart,
    T_LeftRoom, 
    T_JoinAsSpectator,
    T_FriendsListRequest,
    T_FriendsListResponse,
    T_InvitedFriend,
    T_RoomInvitation
};

struct RoomInfo {
    int roomID;
    int currentPlayersCount;
    bool isStarted;
    Modes mode;
    int maxPlayers;
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
