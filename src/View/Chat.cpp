#include "Chat.hpp"

Chat::Chat(){};
Chat::~Chat(){};

void Chat::chatWith(const std::string& user, const string& Friend){}
    /*erase();
    printw("Press ENTER to send the message\nPress 'ENTER' with an empty entry to quit\n");
    printw("\nDiscussion with %s : \n", Friend.c_str());
    refresh();
    echo();
    curs_set(1);
    nodelay(stdscr, false);
    while(true){
        char buffer[200];
        printw("%s : ", user.c_str());
        refresh();
        wgetnstr(stdscr,buffer,199);
        if(strlen(buffer)==0) return;

        ssize_t bytesWritten = write(sockfd , buffer , strlen(buffer));
        if (bytesWritten < 0){
            if (errno == EPIPE){
                perror("Serveur est déconnecté");
                return -1;}
            else{
                perror("Erreur d'écriture");
                return -2;}
    }
}*/