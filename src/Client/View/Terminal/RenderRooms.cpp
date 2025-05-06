#include "RenderRooms.hpp"
#include <vector>

RenderRooms::RenderRooms(){}

void RenderRooms::myRoom(const RoomInfo room)
{
    erase();

    printw("==========  My Room  ==========\n\n");

    printw(" Room ID: %d\n", room.roomID);
    printw(" Current Players: %d\n", room.currentPlayersCount);
    printw(" Has Started: %s\n", (room.isStarted ? "YES" : "NO"));
    printw("\n--------------------------------\n");
    printw(" [Instructions]\n");
    printw("  - Press 'f' to force-start (only if you are the room owner).\n");
    printw("  - Press 'q' to go back/leave the room.\n");
    printw("  - Press 'r' to refresh.\n");

    refresh();
}

void RenderRooms::listRooms(const std::vector<RoomInfo>& roomsInfo, int highlightIndex)
{
    erase();
    printw("=== AVAILABLE ROOMS ===\n\n");
    
    if (roomsInfo.empty()) {
        printw("No rooms available.\n\n");
        printw("Press 'q' to go back.\n");
        refresh();
        return;
    }

    for (int i = 0; i < (int)roomsInfo.size(); i++) {
        const auto& r = roomsInfo[i];
        
        if (i == highlightIndex) {
            attron(A_STANDOUT);
            printw("-> RoomID=%d, players=%d, started=%s\n",
                   r.roomID, r.currentPlayersCount,
                   (r.isStarted ? "YES" : "NO"));
            attroff(A_STANDOUT);
        } else {
            printw("   RoomID=%d, players=%d, started=%s\n",
                   r.roomID, r.currentPlayersCount,
                   (r.isStarted ? "YES" : "NO"));
        }
    }

    printw("\nUse UP/DOWN to navigate, [ENTER] to select, [r] to refresh, [q] to cancel, [s] to spectate\n");
    refresh();
}


int RenderRooms::handleMyRoom(const RoomInfo room) {
    myRoom(room);
    int ch = getch();
    switch (ch) {
        case 'f':
            return 1;
        case 'q':
        case 'Q':
            return -1;
        case 'r':
            return -2;
        default:
            break;
    }
    return -3;
}


int RenderRooms::handleListRooms(std::vector<RoomInfo> rooms, int& highlightIndex) {
    listRooms(rooms, highlightIndex);

    int ch = getch();
    switch (ch) {
        case KEY_UP:
            if (highlightIndex > 0) {
                highlightIndex--;
            }
            break;
        case KEY_DOWN:
            if (highlightIndex < (int)rooms.size() - 1) {
                highlightIndex++;
            }
            break;
        case '\n':
            if(rooms.size() > 0)return highlightIndex;
        case 's':
            if(rooms.size() > 0)return highlightIndex + (int)rooms.size();
        case 'q':
        case 'Q':
            return -1;
        case 'r':
            return -2;
        default:
            return -3;
    }

return -3;
}

void RenderRooms::spectatorRoom(const RoomInfo& room) {
    erase();
    printw("===== Spectator Waiting Room =====\n\n");
    printw(" Room ID: %d\n",               room.roomID);
    printw(" Current Players: %d\n",       room.currentPlayersCount);
    printw(" Game Started: %s\n",
           room.isStarted ? "YES" : "NO");
    printw("\n[Instructions]\n");
    printw(" - Press 'r' to refresh.\n");
    printw(" - Press 'q' to stop spectating.\n");
    if (room.isStarted) {
        printw("\nGame is starting… just sit tight!\n");
    }
    refresh();
}


int RenderRooms::handleSpectatorRoom(const RoomInfo& room) {
    spectatorRoom(room);
    int ch = getch();
    switch (ch) {
        case 'r': case 'R':
            return -2;
        case 'q': case 'Q':
            return -1;
        default:
            return 0;
    }
}