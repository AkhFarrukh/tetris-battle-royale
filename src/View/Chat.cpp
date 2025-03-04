//
// Created by hamza on 4/03/25.
//

#include "Chat.hpp"

Chat::Chat(){};
Chat::~Chat(){};

void Chat::chatWith(const std::string& user, const string& Friend){
    erase();
    printw("Press ENTER to send the message\nPress 'ENTER' with an empty entry to quit\n");
    printw("\nDiscussion with %s : \n", Friend.c_str());
    refresh();
    echo();
    curs_set(1);
    nodelay(stdscr, false);
    while(true){
        char buffer[200];
        printw("\n%s : ", user.c_str());
        refresh();
        wgetnstr(stdscr,buffer,199);
        if(strlen(buffer)==0) return;
    }
}