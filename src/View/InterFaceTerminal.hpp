#ifndef INTERFACETERMINAL_HPP
#define INTERFACETERMINAL_HPP

#include <iostream>
#include "game.hpp"
#include "MenuGameMode.hpp"
#include "MenuGameModeController.hpp"
#include "../Database/database.hpp"

class InterfaceTerminal {

  private:
    static const char* modeNames[];  // tableau qui va contenir les noms des diffents modes de jeu
                                     // static pour pouvoir partager dans toutes la classe sans dupliquer de memoire
    static const char* friendsOptions[]; // tableau qui va contenir les options pour les amis
                                         // static pour pouvoir partager dans toutes la classe sans dupliquer de memoire

public:
  InterfaceTerminal();
//  InterfaceTerminal(InterfaceTerminal &&) = default;
//  InterfaceTerminal(const InterfaceTerminal &) = default;
//  InterfaceTerminal &operator=(InterfaceTerminal &&) = default;
//  InterfaceTerminal &operator=(const InterfaceTerminal &) = default;
  ~InterfaceTerminal();

  void initNcurses();
  void displayMenu(int selectedOption);
  void displayFriendOptions(int selectedOption, const string friends);
  void displayGameModeMenu(int selectedMode);
  void handleGameModeSelection();
  void displayFriendsMenu(int selectedOption, vector<string> friends);
  void handleFriends();
  void handleFriendsOptionSelection(const string friendName);
  void handleOptionSelection(int selectedOption);
  void enterName();
  void displayFriendRequests(int selectedOption, vector<string> requestsReceived, vector<string> requestsSent);
  void handleFriendRequests();

  void start();
};

#endif