#include "InterFaceTerminal.hpp"
#include <ncurses.h>
#include <locale.h>  // Needed for wide characters (if printing Unicode blocks)



// TODO ------  Docstrings à modofier quand les fonctionnalités seront disponiibles ------


// tableau déclaré static dans fichier .hpp, pour permettre economie de memoire et partage dans la classe
// voir fichier Hpp

Database db("tetris.db");

const char* user = "Luc";

const char* InterfaceTerminal::modeNames[] = {
                                            "Endless Mode",
                                            "Classic Mode",
                                            "Duel Mode",
                                            "Royal Competition Mode"
                                            };

const char* InterfaceTerminal::friendsOptions[] = {
                                            "Remove from friends list",
                                            "Send a message (Not available yet)",
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
                                "Handle friends",
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
 *@brief Gère la sélection et l'exécution des modes de jeu.
 *
 *Cette méthode permet de choisir un mode de jeu (via le menu des modes).
 *L'utilisateur parcourt avec les flèches haut/bas et sélectionne avec Entrée.
 *Seulement "Endless Mode" est fonctionnel pour l'instant.
 *
 */
void InterfaceTerminal::handleGameModeSelection() {
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


void InterfaceTerminal::displayFriendsMenu(int selectedOption, vector<string> friends) {
    erase();
    printw("Friends list : \n\n");
    int numberFriends = std::size(friends);   // taille du tableau contenant les amis
    if(numberFriends == 0) {printw("Empty friends list \n");}
    else{
    for (int i = 0; i < numberFriends; ++i) {
        if (i == selectedOption) {
            printw("-> %d. %s", i + 1, friends[i].c_str());
        } else {
            printw("   %d. %s", i + 1, friends[i].c_str());
        }
        printw("\n");
    }
    }
    if (selectedOption == numberFriends) printw("\n-> %s\n", "Add new friend");
    else printw("\n   %s\n", "Add new friend");
    if (selectedOption == numberFriends+1) printw("-> Friend requests(%d)\n", db.getNumberFriendRequestsReceived(user));
    else printw("   Friend requests(%d)\n", db.getNumberFriendRequestsReceived(user));

    printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select an option\nPress 'q' to quit\n");
    refresh();
}


void InterfaceTerminal::displayFriendOptions(int selectedOption, const string friendsName) {
    erase();
    printw("Choose an action to do with %s:\n\n", friendsName.c_str());
    int numberFriendsOptionsPossible = std::size(friendsOptions);   // taille du tableau contenant les modes de jeu

    for (int i = 0; i < numberFriendsOptionsPossible; ++i) {
        if (i == selectedOption) {
            printw("-> %d. %s", i + 1, friendsOptions[i]);
        } else {
            printw("   %d. %s", i + 1, friendsOptions[i]);
        }
        printw("\n");
    }
    printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select an option\nPress 'q' to quit\n");
    refresh();
}

void InterfaceTerminal::enterName(){
    erase();
    printw("Press ENTER to research the player name\nPress 'ENTER' with an empty entry to quit\n");
    printw("\nEnter the player name : \n");
    refresh();
    echo();
    curs_set(1);
    nodelay(stdscr, false);
    int returnValue;

    char namePlayer[100];

    while (true) {
        wgetnstr(stdscr,namePlayer,99);
        if(strlen(namePlayer)==0) return;
        if(strcmp(namePlayer, user)==0) printw("\nPlease enter a name different from yours ! \n");
        else {
            printw("\nResearch player... \n");
            refresh();
            returnValue = db.verifyRequestFriend(user, namePlayer);
            if(returnValue==0){db.requestFriend(user, namePlayer);napms(1000); printw("\nFriend request sent ! \n");
                                                                                          refresh();napms(1000);}
            else if(returnValue==2) printw("\n%s is already you friend \n", namePlayer);
            else if(returnValue==3) printw("\nYou have already sent a friend request to %s \n", namePlayer);
            else if(returnValue==4){db.deleteRequestFriend(user, namePlayer); db.addFriend(user, namePlayer);
              printw("\n%s had already friend-requested you, so now you are friend !\n", namePlayer);
                                                                                         refresh();napms(1500);}
            else printw("\nInvalid username ! \n");}
}
}

void InterfaceTerminal::displayFriendRequests(int selectedOption, vector<string> requestsReceived, vector<string> requestsSent) {
    erase();
    printw("Requests received :\n\n");
    int numberRequestsReceived = std::size(requestsReceived);   //taille du tableau contenant les demandes d'amis recues
    int numberRequestsSent = std::size(requestsSent);   // taille du tableau contenant les demandes d'amis envoyées
    if(numberRequestsReceived==0) printw("No request received\n");
    for (int i = 0; i < numberRequestsReceived; ++i) {
        if (i == selectedOption) {
            printw("-> %s wants to be your friend !",  requestsReceived[i].c_str());
        } else {
            printw("   %s wants to be your friend !",  requestsReceived[i].c_str());
        }
        printw("\n");
    }
    printw("\nRequests sent :\n\n");
    if(numberRequestsSent==0) printw("No request sent\n");
    for (int i = 0; i < numberRequestsSent; ++i) {
        if (i+numberRequestsReceived == selectedOption) {
            printw("-> friend request sent to %s ", requestsSent[i].c_str());
        } else {
            printw("   friend request sent to %s", requestsSent[i].c_str());
        }
        printw("\n");
    }
    printw("\nUse UP and DOWN arrow keys to navigate\nPress 'A' to accept the request\nPress 'R' to refuse\nPress 'C' to cancel a sent friend request\nPress 'q' to quit\n");
    refresh();
}


void InterfaceTerminal::handleFriendRequests() {
    int selectedOption = 0;

    while (true) {

        vector<string> requestsReceivedList = db.requestReceivedList(user);
        vector<string> requestsSentList = db.requestSentList(user);
        int numberRequestReceived = std::size(requestsReceivedList);
        int numberRequestSent = std::size(requestsSentList);
        displayFriendRequests(selectedOption, requestsReceivedList, requestsSentList);  // affiche le menu avec les demandes

        int ch = getch();    // récupère l'entrée utilisateur
        switch (ch) {

            case KEY_UP:     // aller vers le haut
                if (selectedOption > 0) {
                    selectedOption--;
                }
            break;
            case KEY_DOWN: // descendre
                if (selectedOption < numberRequestSent +numberRequestReceived-1) {
                    selectedOption++;
                }
            break;

            // touche 'entrée'
            case 'A':
            case 'a':
              if(selectedOption < numberRequestReceived){
                db.addFriend(user, requestsReceivedList[selectedOption]);
                db.deleteRequestFriend(user, requestsReceivedList[selectedOption]);
              }
            break;
            case 'R':
            case 'r':
              if(selectedOption < numberRequestReceived){
                  db.deleteRequestFriend(user, requestsReceivedList[selectedOption]);
              }
            break;
            case 'C':
            case 'c':
              if(numberRequestReceived <= selectedOption && selectedOption < numberRequestSent){
                  db.deleteRequestFriend(user, requestsSentList[selectedOption]);
              }
            break;
            case 'q':
            case 'Q':
                endwin();
            return;
        }
    }
}


void InterfaceTerminal::handleFriends() {
    int selectedOption = 0;

    while (true) {

        vector<string> friendsList = db.friendsList(user);
        displayFriendsMenu(selectedOption, friendsList);  // affiche le menu avec les options

        int ch = getch();    // récupère l'entrée utilisateur
        switch (ch) {

            case KEY_UP:     // aller vers le haut
                if (selectedOption > 0) {
                    selectedOption--;
                }
            break;
            case KEY_DOWN: // descendre
                if (selectedOption <= friendsList.size()) {
                    selectedOption++;
                }
            break;

            // touche 'entrée'
            case '\n':
                if (selectedOption < friendsList.size()) { //On a cliqué sur un des amis
                    handleFriendsOptionSelection(friendsList[selectedOption]);
                }
                else if (selectedOption == friendsList.size()) { //on a cliqué sur ajouter un ami
                    enterName();
                    noecho();
                    curs_set(0);
                    nodelay(stdscr, true);
                }
                else { //Demande d'amis recus
                    erase();
                    handleFriendRequests();
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
 *@param selectedOption Indice de l'action sélectionnée.
 */
void InterfaceTerminal::handleOptionSelection(int selectedOption) {
   switch (selectedOption) {
       case 0:
            // Si l'utilisateur sélectionne "Play", on affiche le menu des modes de jeu.
           handleGameModeSelection();
           break;

       case 1:
           // Si l'utilisateur sélectionne "Friends", on affiche les options possibles avec les amis.
           handleFriends();
           break;

   }
}


void InterfaceTerminal::handleFriendsOptionSelection(const string friendName) {
    int selectedOption = 0;

    while (true) {
        displayFriendOptions(selectedOption, friendName);

        int ch = getch();    // récupère l'entrée utilisateur
        switch (ch) {

            case KEY_UP:     // aller vers le haut
                if (selectedOption > 0) {
                    selectedOption--;
                }
            break;
            case KEY_DOWN: // descendre
                if (selectedOption < 1) {
                    selectedOption++;
                }
            break;

            // touche 'entrée'
        case 'q':
        case 'Q':
               endwin();
            return;
        case '\n':
        switch (selectedOption) {
        case 0:
            // Si l'utilisateur sélectionne "Remove Player", on retire l'amis de la liste d'amis.
                db.deleteFriend(user, friendName);
            erase();
            printw("Friend removed !\n");
            refresh();
            napms(700);
            return;
        break;

        case 1:
            // Si l'utilisateur sélectionne "Send message", on envoie un message à l'ami.
                handleFriends();
        break;
    }
  }
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
               if (selectedOption < 1) {
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
