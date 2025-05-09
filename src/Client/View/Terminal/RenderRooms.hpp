#ifndef RENDERROOMS_HPP
#define RENDERROOMS_HPP

#include "ServerData.hpp"
#include "Communication.hpp"
#include <ncurses.h>
#include <memory>
#include <vector>

class RenderRooms {
    private:

    public:
        RenderRooms();
        void myRoom(const RoomInfo room);
        void listRooms(const std::vector<RoomInfo>& roomsInfo, int hover);
        int handleListRooms(std::vector<RoomInfo> roomsInfo, int& highlightIndex);
        int handleMyRoom(const RoomInfo room);
        void spectatorRoom(const RoomInfo& room);
        int handleSpectatorRoom(const RoomInfo& room);
        int selectMaxPlayers();
        void displaySMP(int highlight);


};




#endif