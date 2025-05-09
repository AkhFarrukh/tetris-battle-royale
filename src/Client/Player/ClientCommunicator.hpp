#ifndef CLIENTCOMM_HPP
#define CLIENTCOMM_HPP
#include "Communication.hpp"

class ClientCommunicator {
private:
    int sock_;

public:
    ClientCommunicator() {}
    ClientCommunicator(int sock): sock_(sock) {}

    void setSock(int socket) {
        sock_ = socket;
    }

    bool sendMenuRequest(MenuActions action, Modes mode) {
        MenuMessage msg;
        msg.action = action;
        msg.mode   = mode;

        int type = MessageType::T_Menu;
        if (!sendAll(sock_, &type, sizeof(type)))   return false;
        int size = sizeof(msg);
        if (!sendAll(sock_, &size, sizeof(size)))    return false;
        if (!sendAll(sock_, &msg, sizeof(msg)))      return false;
        return true;
    }
};


#endif