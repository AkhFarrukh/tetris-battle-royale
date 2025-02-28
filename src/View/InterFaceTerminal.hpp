#ifndef INTERFACETERMINAL_HPP
#define INTERFACETERMINAL_HPP

#include <iostream>
#include "game.hpp"
#include "MenuGameMode.hpp"
#include "MenuGameModeController.hpp"

class InterfaceTerminal {

  private:
    static const char* modeNames[];  // tableau qui va contenir les noms des diffents modes de jeu 
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
  void displayGameModeMenu(int selectedMode);
  void handleGameModeSelection();
  void handleOptionSelection(int selectedOption);

  void start();
};

#endif