#ifndef INTERFACETERMINAL_HPP
#define INTERFACETERMINAL_HPP

#include <iostream>
#include "game.hpp"
#include "database.hpp" //changer en "database.hpp" si probleme
#include "userdatabasemanager.hpp"
#include "Chat.hpp"
#include <unistd.h>
#include <mutex>
#include <atomic>
#include "menu.hpp"

extern std::mutex interfaceMutex;

class InterfaceTerminal {

  private:

    // UserDatabaseManager Class already has DB initialised.
    // NO NEED to construct the separate the Database object
    UserDatabaseManager udm;

    std::string user = "";
    int sock_;

    // static pour pouvoir partager dans toutes la classe sans dupliquer de memoire
    static const char* modeNames[];      // tableau qui va contenir les noms des diffents modes de jeu
    static const char* friendsOptions[]; // tableau qui va contenir les options pour les amis

  public:
    InterfaceTerminal();
    ~InterfaceTerminal();

    void initNcurses();
    void displayMenu(int selectedOption) const;
    void displayConnexionOption(int selectedOption);
    vector<std::string> handleConnexionOption();
    vector<std::string> handleLogin();
    vector<std::string> handleRegister();
    int handleMessageBox();
    void displayMessageBox(int selectedOption, vector<string> friends);
    int getSock(){return sock_;};
    void setSock(int sock){sock_ = sock;};
    int chatWith(const std::string& user, const string& Friend);

    void displayFriendOptions(int selectedOption);
    void displayGameModeMenu(int selectedMode) const;
    void displayMultiplayerOption(int selectedMode) const;

    int handleGameModeSelection(int& selectedMode);
    void displayFriendsMenu(int selectedOption, vector<string> friends);
    int handleFriends();
    int handleFriendsOptionSelection(const string friendName);
    int handleOptionSelection(int &selectedOption);
    void enterName();
    void displayFriendRequests(int selectedOption, vector<string> requestsReceived, vector<string> requestsSent);
    int handleFriendRequests();
    void displayMessageNoRead(const std::string& user, const string& friendName);

    void displayLeaderboard();

    int start();
};

#endif