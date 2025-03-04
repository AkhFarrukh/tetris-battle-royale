#ifndef INTERFACETERMINAL_HPP
#define INTERFACETERMINAL_HPP

#include <iostream>
#include "game.hpp"
#include "MenuGameMode.hpp"
#include "MenuGameModeController.hpp"
#include "../Database/database.hpp" //changer en "database.hpp" si probleme
#include "../Model/userdatabasemanager.hpp"
#include "Chat.hpp"

class InterfaceTerminal {

  private:
    static const char* modeNames[];  // tableau qui va contenir les noms des diffents modes de jeu
                                     // static pour pouvoir partager dans toutes la classe sans dupliquer de memoire
    static const char* friendsOptions[]; // tableau qui va contenir les options pour les amis
                                         // static pour pouvoir partager dans toutes la classe sans dupliquer de memoire
    int sock_;
public:
  InterfaceTerminal(int sock);
//  InterfaceTerminal(InterfaceTerminal &&) = default;
//  InterfaceTerminal(const InterfaceTerminal &) = default;
//  InterfaceTerminal &operator=(InterfaceTerminal &&) = default;
//  InterfaceTerminal &operator=(const InterfaceTerminal &) = default;
  ~InterfaceTerminal();

  void initNcurses();
  void displayMenu(int selectedOption) const;
  void displayConnexionOption(int selectedOption);
  void handleConnexionOption();
  void handleLogin();
  void handleRegister();
  void handleMessageBox();
  void displayMessageBox(int selectedOption, vector<string> friends);
  int getSock(){return sock_;};

  void displayFriendOptions(int selectedOption);
  void displayGameModeMenu(int selectedMode) const;
  void handleGameModeSelection(int& selectedMode);
  void displayFriendsMenu(int selectedOption, vector<string> friends);
  void handleFriends();
  void handleFriendsOptionSelection(const string friendName);
  void handleOptionSelection(int &selectedOption);
  void enterName();
  void displayFriendRequests(int selectedOption, vector<string> requestsReceived, vector<string> requestsSent);
  void handleFriendRequests();

  int start();
};

#endif