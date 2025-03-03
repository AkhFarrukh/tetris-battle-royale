#include "InterFaceTerminal.hpp"
#include <ncurses.h>
#include <locale.h>  // Needed for wide characters (if printing Unicode blocks)



// TODO ------  Docstrings à modofier quand les fonctionnalités seront disponiibles ------
// TODO ------- ENLEVER '(Unreleased)' aux modes de jeu, quand on pourra jouer à tous les modes.


// tableau déclaré static dans fichier .hpp, pour permettre economie de memoire et partage dans la classe
// voir fichier Hpp
const char* InterfaceTerminal::modeNames[] = { 
                                            "Endless Mode",  
                                            "Classic Mode (Unreleased)",  
                                            "Duel Mode (Unreleased)",  
                                            "Royal Competition Mode (Unreleased)"  
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
   init_pair(7, COLOR_BLACK, COLOR_WHITE);  // Texte noir sur fond blanc

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
 *@brief Affiche le menu principal et l'option sélectionnée.
 * 
 *Affichage réalisé en parcourant les options du menu,
 *et une petite flèche pointe vers l'option actuelle.
 *
 *@param selectedOption Indice de l'option actuellement sélectionnée.
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
           attron(COLOR_PAIR(7));    // activer couleur noir sur blanc pour option selected
           printw("-> %d. %s\n", i + 1, menuOptions[i]);
           attroff(COLOR_PAIR(7));  
       } else {
           printw("   %d. %s\n", i + 1, menuOptions[i]);
       }
   }
   printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select an action\nPress 'q' to quit\n");
   refresh();
}




/**
 *@brief Affiche le menu de sélection des modes de jeu.
 *
 *Modes de jeu sont parcourus et affichés avec une petite
 *flèche pointant vers le mode actuel.
 *
 *@param selectedMode Indice du mode de jeu actuellement sélectionné.
 */
void InterfaceTerminal::displayGameModeMenu(int selectedMode) {
   erase();
   printw("Select a game mode then press Enter to start:\n\n");
   int numberModePossible = std::size(modeNames);   // taille du tableau contenant les modes de jeu
   
   for (int i = 0; i < numberModePossible; ++i) {
       if (i == selectedMode) {
           attron(COLOR_PAIR(7));   // activer couleur noir sur blanc pour mode selectionné
           printw("-> %d. %s", i + 1, modeNames[i]);
           attroff(COLOR_PAIR(7));
       } else {
           printw("   %d. %s", i + 1, modeNames[i]);
       }
       printw("\n");
   }
   printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select a mode\nPress 'q' to quit\n");
   refresh();
}




/**
 *@brief Gère la sélection et l'exécution des modes de jeu.
 *
 *Cette méthode permet de choisir un mode de jeu (via le menu des modes).
 *L'utilisateur parcourt avec les flèches haut/bas et sélectionne avec Entrée.
 *Seulement "Endless Mode" est fonctionnel pour l'instant.
 *
 */
int InterfaceTerminal::handleGameModeSelection() {
   int selectedMode = 0;
   
   while (true) {
       
    displayGameModeMenu(selectedMode);  // affiche le menu avec les modes 
       
    int ch = getch();    // récupère l'entrée utilisateur
       switch (ch) {  
        
        case KEY_UP:     // aller vers le haut
               if (selectedMode > 0) {
                   selectedMode--;
               }
               break;
           case KEY_DOWN: // descendre
               if (selectedMode < 3) {
                   selectedMode++;
               }
               break;
            
            // touche 'entrée'
            case '\n':
               if (selectedMode == 0) {
                   erase();
                   printw("Endless Mode selected! Starting game...\n");
                   refresh();
                   napms(500);
                   //MenuGameModeController::startEndlessMode();  // lance le jeu en mode Endless
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
 *@brief Gère la sélection des options dans le menu principal.                                            
 *
 *Seulement la premiere ioption est fonctionnelle pour l'instant: 'Play' (case 0)
 *Non-Fonctionnels:
 *  - (case 1): 'add a friend'
 *  - (case 2): 'delete a friend'
 *  - (case 3): 'show my friend's list'
 *
 *@param selectedOption Indice de l'action sélectionnée.
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
 *@brief Démarre l'interface terminal utilisateur et 
 *gère les interactions utilisateur dans
 *le menu principal.
 *
 */
void InterfaceTerminal::start() {
   initNcurses();
   int selectedOption = 0;
   
   while (true) {
       displayMenu(selectedOption);
       int ch = getch();
       switch (ch) {
           
           case KEY_UP:   // monter
               if (selectedOption > 0) {
                   selectedOption--;
               }
               break; 
           case KEY_DOWN:  // descendre
               if (selectedOption < 3) {
                   selectedOption++;
               }
               break;
           case '\n':   // touche 'entrée'
               handleOptionSelection(selectedOption);
               break;
           case 'q':
           case 'Q':
               endwin();
               return;
       }
   }
}
