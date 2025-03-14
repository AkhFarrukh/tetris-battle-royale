#ifndef RENDERROOMS_HPP
#define RENDERROOMS_HPP

#include "ServerData.hpp"
#include <ncurses.h>

class RenderRooms {
    private:

    public:
        RenderRooms();
        void myRoom(const RoomInfo room);
        void listRooms(const std::vector<RoomInfo>& roomsInfo, int hover);
        int handleListRooms(std::vector<RoomInfo> roomsInfo);
        int handleMyRoom(const RoomInfo room);

};




#endif