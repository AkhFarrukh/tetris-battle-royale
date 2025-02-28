#include "InterFaceTerminal.hpp"
#include <ncurses.h>
#include <locale.h>  // Needed for wide characters (if printing Unicode blocks)


// constante statique pour economiser de la mémoire en evitant à chaque fois recreer le tableau
// voir fichier Hpp
const char* InterfaceTerminal::modeNames[] = { 
                                            "Endless Mode",  
                                            "Classic Mode",  
                                            "Duel Mode",  
                                            "Royal Competition Mode"  
                                            };    

InterfaceTerminal::InterfaceTerminal() {}
InterfaceTerminal::~InterfaceTerminal() {}


/**
* @brief Initialise ncurses et configure l'affichage du terminal.
*/
void InterfaceTerminal::initNcurses() {
   setlocale(LC_ALL, "");
   initscr();
   cbreak();
   noecho();
   keypad(stdscr, TRUE);
   curs_set(0);
   nodelay(stdscr, TRUE);
   start_color();
   if (has_colors() && can_change_color()) {
       use_default_colors();
   }
   init_pair(1, COLOR_RED, -1);
   init_pair(2, COLOR_GREEN, -1);
   init_pair(3, COLOR_YELLOW, -1);
   init_pair(4, COLOR_BLUE, -1);
   init_pair(5, COLOR_MAGENTA, -1);
   init_pair(6, COLOR_CYAN, -1);
}

/**
* @brief Affiche le menu principal et l'option sélectionnée.

* @param selectedAction Indice de l'option actuellement sélectionnée.
*/
void InterfaceTerminal::displayMenu(int selectedOption) {
   erase();
   printw("Welcome to Tetris Royale (Terminal Edition)!\n");
   printw("Choose an action: \n\n");
   const char* menuOptions[] = {
                                "Play", 
                                "Add a friend (Not available yet)", 
                                "Delete a friend (Not available yet)", 
                                "Show my friend's list (Not available yet)"
                                };
   
   int numberPossibleOptions = std::size(menuOptions); // taille du tableau contanant options du menu principal
   
   for (int i = 0; i < numberPossibleOptions; ++i) {
       if (i == selectedOption) {
           printw("-> %d. %s\n", i + 1, menuOptions[i]);
       } else {
           printw("   %d. %s\n", i + 1, menuOptions[i]);
       }
   }
   printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select an action\nPress 'q' to quit\n");
   refresh();
}

/**
* @brief Affiche le menu de sélection des modes de jeu.
* @param selectedMode Indice du mode de jeu actuellement sélectionné.
*/
void InterfaceTerminal::displayGameModeMenu(int selectedMode) {
   erase();
   printw("Select a game mode then press Enter to start:\n\n");
   int numberModePossible = std::size(modeNames);   // taille du tableau contenant les modes de jeu
   
   for (int i = 0; i < numberModePossible; ++i) {
       if (i == selectedMode) {
           printw("-> %d. %s", i + 1, modeNames[i]);
       } else {
           printw("   %d. %s", i + 1, modeNames[i]);
       }
       if (i != 0) {
           printw(" (Unreleased)");
       }
       printw("\n");
   }
   printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select a mode\nPress 'q' to quit\n");
   refresh();
}

/**
* @brief Gère la sélection et l'exécution des modes de jeu.
*/
void InterfaceTerminal::handleGameModeSelection() {
   int selectedMode = 0;
   
   while (true) {
       
    displayGameModeMenu(selectedMode);  // affiche le menu avec les modes 
       
    int ch = getch();    // récupère l'entree utilisateur
       switch (ch) {  
        case KEY_UP:
               if (selectedMode > 0) {
                   selectedMode--;
               }
               break;
           case KEY_DOWN:
               if (selectedMode < 3) {
                   selectedMode++;
               }
               break;
            
            // touche 'entré'
            case '\n':
               if (selectedMode == 0) {
                   erase();
                   printw("Endless Mode selected! Starting game...\n");
                   refresh();
                   napms(500);
                   MenuGameModeController::startEndlessMode();  // lance le jeu en mode Endless
                   return;
               
                } else {        // autres modes de jeu, pas encore implémenté      
                   erase();
                   printw("%s is not available yet.\nPress any key to return to menu...\n", modeNames[selectedMode]);
                   refresh();
                   nodelay(stdscr, FALSE);
                   getch();
                   nodelay(stdscr, TRUE);
               }
               break;
           case 'q':
           case 'Q':
               endwin();
               return;
       }
   }
}

/**
 * @brief Gère la sélection des options dans le menu principal.
 * @param selectedAction Indice de l'action sélectionnée.
 */
void InterfaceTerminal::handleOptionSelection(int selectedOption) {
   switch (selectedOption) {
       case 0:
            // Si l'utilisateur sélectionne "Play", on affiche le menu des modes de jeu.
           handleGameModeSelection();
           break;
       
       // dans les trois 'case' suivants, le programme effectue la meme chose  (parce que les fonctionnalités pas encore dispo)
       case 1:
       case 2:
       case 3:
           erase();
           printw("This feature is not available yet.\nPress any key to return to menu...\n");
           refresh();
           nodelay(stdscr, FALSE);
           getch();
           nodelay(stdscr, TRUE);
           break;
   }
}

/**
* @brief Démarre l'interface utilisateur et gère les interactions utilisateur.
*/
void InterfaceTerminal::start() {
   initNcurses();
   int selectedOption = 0;
   
   while (true) {
       displayMenu(selectedOption);
       int ch = getch();
       switch (ch) {
           case KEY_UP:
               if (selectedOption > 0) {
                   selectedOption--;
               }
               break;
           case KEY_DOWN:
               if (selectedOption < 3) {
                   selectedOption++;
               }
               break;
           case '\n':
               handleOptionSelection(selectedOption);
               break;
           case 'q':
           case 'Q':
               endwin();
               return;
       }
   }
}
