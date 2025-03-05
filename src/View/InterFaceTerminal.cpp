#include "InterFaceTerminal.hpp"
#include <ncurses.h>
#include <locale.h>  // Needed for wide characters (if printing Unicode blocks)



// TODO ------  Docstrings à modofier quand les fonctionnalités seront disponiibles ------


// tableau déclaré static dans fichier .hpp, pour permettre economie de memoire et partage dans la classe
// voir fichier Hpp

Database db("tetris.db");
UserDatabaseManager udm;
Chat chat;


std::string user = "";

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

void InterfaceTerminal::initNcurses() {
    setlocale(LC_ALL, "");
    // Initialize ncurses
    initscr();            // Start curses mode
    cbreak();             // Line buffering disabled
    noecho();             // Don't echo() while we do getch
    keypad(stdscr, TRUE); // We get F1, F2 etc..
    curs_set(0);          // Hide the cursor
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

    const int numModes = 4;
    int selectedMode = 0;  // Tracks the currently selected menu option
}

/**
 *@brief Affiche le menu principal et l'option sélectionnée.
 *
 *Affichage réalisé en parcourant les options du menu,
 *et une petite flèche pointe vers l'option actuelle.
 *
 *@param selectedOption Indice de l'option actuellement sélectionnée.
 */
void InterfaceTerminal::displayMenu(int selectedOption) const {
   erase();
   printw("Welcome to Tetris Royale (Terminal Edition)!\n");
   printw("Choose an action: \n\n");
   const char* menuOptions[] = {
                                "Play",
                                "Handle friends",
                                "Account (currently being updated)"
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
   if(!(user==""))printw("Press 't' to open the message box\n");
   refresh();
}


void InterfaceTerminal::displayMessageBox(int selectedOption, vector<string> friends) {
    erase();
    printw("Message box : \n\n");
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

    printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select a friend\nPress 'q' to quit\n");
    refresh();
}



int InterfaceTerminal::handleMessageBox() {
    int selectedOption = 0;
    while(user==""){
        erase();
        printw("Log in or create a new account to chat with your friends !\n");
        printw("\nPress 'q' to quit\n");
        int ch = getch();
        switch(ch){
            case 'q':
            case 'Q':
                endwin();
            return 0;}
    }
    while (true) {

        vector<string> friendsList = db.friendsList(user);
        displayMessageBox(selectedOption, friendsList);  // affiche le menu avec les options

        int ch = getch();    // récupère l'entrée utilisateur
        switch (ch) {

            case KEY_UP:     // aller vers le haut
                if (selectedOption > 0) {
                    selectedOption--;
                }
            break;
            case KEY_DOWN: // descendre
                if (selectedOption < friendsList.size()-1) {
                    selectedOption++;
                }
            break;

            // touche 'entrée'
            case '\n':
                    if(chatWith(user, friendsList[selectedOption])<0)return -1;
                    scrollok(stdscr, false);
                    noecho();
                    curs_set(0);
                    nodelay(stdscr, true);
            break;
            case 'q':
            case 'Q':
                endwin();
            return 0;
        }
    }
}


int InterfaceTerminal::chatWith(const std::string& user, const string& friendName){
    erase();
    printw("Press ENTER to send the message\nPress 'ENTER' with an empty entry to quit\n");
    printw("\nDiscussion with %s : \n", friendName.c_str());
    refresh();
    echo();
    curs_set(1);
    nodelay(stdscr, false);
    bool windowFull = false;
    int numberLines = LINES;
    while(true){
        if (windowFull) {
            move(3, 0);clrtoeol();
            move(2, 0);clrtoeol();
            move(1, 0);clrtoeol();
            move(0, 0);clrtoeol();
            printw("Press ENTER to send the message\nPress 'ENTER' with an empty entry to quit\n");
            printw("\nDiscussion with %s : \n", friendName.c_str());
        }
        char buffer[1024];
        move(numberLines - 1, 0);
        clrtoeol();
        printw("Enter your message : ");
        refresh();
        wgetnstr(stdscr,buffer,1023);
        if(strlen(buffer)==0) return 0;
        else{
        ///
        /// MUTEX DEB
        curs_set(0);
        int currentLine;
        if(true){
        currentLine = numberLines-1;
        while(mvinch(currentLine-1, 0) == ' ' || mvinch(currentLine-1, 0) == ERR){ //tant que ligne est vide
            currentLine--;
            }
        }
        move(currentLine, 0);
        printw("%s : %s\n", user.c_str(), buffer);
        if(!(udm.isLoggedIn(friendName.c_str())))printw("Message not delivered, %s is offline", friendName.c_str());
        refresh();
        if(numberLines-1-currentLine < 5){
          windowFull = true;
          move(numberLines-1, 0);
          clrtoeol();
          scrollok(stdscr, true);
          scroll(stdscr);
          scrollok(stdscr, false);
        }
        refresh();
        curs_set(1);

        /// MUTEX FIN
        ///
        int socket = getSock();
        std::vector<char> message(friendName.size()+1+strlen(buffer));
    	memcpy(message.data(), friendName.c_str(), friendName.size()+1);
        memcpy(message.data() + friendName.size()+1, buffer, strlen(buffer));
        ssize_t bytesWritten = write(socket , message.data() , friendName.size()+1+strlen(buffer));
        if (bytesWritten <= 0){
            if (errno == EPIPE){
                perror("Serveur est déconnecté");}
            else{
                perror("Erreur d'écriture");}
            return -1;
        }
        else{memset(buffer, 0, sizeof(buffer));}

        }
    }
    return 0;
}

/**
 *@brief Affiche le menu de sélection des modes de jeu.
 *
 *Modes de jeu sont parcourus et affichés avec une petite
 *flèche pointant vers le mode actuel.
 *
 *@param selectedMode Indice du mode de jeu actuellement sélectionné.
 */
void InterfaceTerminal::displayGameModeMenu(int selectedMode) const {
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
   if(!(user==""))printw("Press 't' to open the message box\n");
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
int InterfaceTerminal::handleGameModeSelection(int& selectedMode) {
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
                   /* MODIF SERVER */
                   return 0;

                } else {        // autres modes de jeu, pas encore implémenté
                   erase();
                   printw("%s is not available yet.\nPress any key to return to menu...\n", modeNames[selectedMode]);
                   refresh();
                   nodelay(stdscr, FALSE);
                   getch();
                   nodelay(stdscr, TRUE);
               }
               break;
           case 't':
           case 'T':
               if(!(user==""))if(handleMessageBox()<0)return -1;
            break;
           case 'q':
           case 'Q':
               endwin();
               return 0;
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
    if(!(user==""))printw("Press 't' to open the message box\n");
    refresh();
}


void InterfaceTerminal::displayFriendOptions(int selectedOption) {
    erase();
    printw("Choose an action to do with :\n\n");
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
    if(!(user==""))printw("Press 't' to open the message box\n");
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
        if(strcmp(namePlayer, user.c_str())==0) printw("\nPlease enter a name different from yours ! \n");
        else {
            printw("\nResearch player... \n");
            refresh();
            returnValue = db.verifyRequestFriend(user, namePlayer);
            if(returnValue==0){if(db.requestFriend(user, namePlayer))printw("\nFriend request sent ! \n");
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
    if(!(user==""))printw("Press 't' to open the message box\n");
    refresh();
}


int InterfaceTerminal::handleFriendRequests() {
    int selectedOption = 0;

    while (true) {

        vector<string> requestsReceivedList = db.requestReceivedList(user);
        vector<string> requestsSentList = db.requestSentList(user);
        int numberRequestReceived = std::size(requestsReceivedList);
        int numberRequestSent = std::size(requestsSentList);
        displayFriendRequests(selectedOption, requestsReceivedList, requestsSentList);  //affiche le menu avec les demandes

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
            case 't':
            case 'T':
               if(!(user==""))if(handleMessageBox()<0)return -1;
            break;
            case 'q':
            case 'Q':
                endwin();
            return 0;
        }
    }
}


int InterfaceTerminal::handleFriends() {
    int selectedOption = 0;
    while(user==""){
        erase();
        printw("Log in or create a new account to play with your friends !\n");
        printw("\nPress 'q' to quit\n");
        int ch = getch();
        switch(ch){
        case 'q':
        case 'Q':
            endwin();
        return 0;}
    }
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
            case 't':
            case 'T':
               if(!(user==""))if(handleMessageBox()<0)return -1;
            break;
            case 'q':
            case 'Q':
                endwin();
            return 0;
        }
    }
}


void InterfaceTerminal::displayConnexionOption(int selectedOption) {
    erase();
    const char* connexionOptions[] = {
        "Log in",
        "Register",
        };
    for (int i = 0; i < std::size(connexionOptions); ++i) {
        if(i==0) printw("If you have already an account, log in here :\n");
        if(i==1) printw("If you don't have an account an account yet, register here :\n");
        if (i == selectedOption) {
            printw("-> %s",  connexionOptions[i]);
        } else {
            printw("   %s",  connexionOptions[i]);
        }
        printw("\n");
    }
    printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select an option\nPress 'q' to quit\n");
    refresh();
}


vector<std::string> InterfaceTerminal::handleLogin(){
    vector<std::string> connexionData;
    curs_set(1);
    nodelay(stdscr, false);
    bool returnValue;
    while (true) {
        erase();
        echo();
        clrtobot();
        bool firstTry = true;
        printw("Press ENTER to validate an entry\nPress ENTER with an empty entry to go back\n");
        refresh();
        printw("\nEnter your username : ");
        move(3,22);
        char namePlayer[100];
        wgetnstr(stdscr,namePlayer,99);
        if(strlen(namePlayer)==0) break;
        while (true) {
            if(firstTry) printw("\nEnter your password : ");
            noecho();
            char passwordPlayer[100];
            wgetnstr(stdscr,passwordPlayer,99);
            if(strlen(passwordPlayer)==0){firstTry = true; break;}
            firstTry = false;
            if(udm.isCorrectLogin(namePlayer, passwordPlayer)){
               napms(1000);
               connexionData.push_back(namePlayer);
               connexionData.push_back(passwordPlayer);
               //printf("%s, %s\n", connexionData[0].c_str(),connexionData[1].c_str());
               user = namePlayer;
               printw("\nConnected ! \n");
               refresh();
               napms(1000);
               return connexionData;
            }
            else {printw("\nIncorrect username or password. \n"); move(5,22);}
            }
    }
    return connexionData;
}


vector<std::string> InterfaceTerminal::handleRegister(){
    vector<std::string> connexionData;
    curs_set(1);
    nodelay(stdscr, false);
    bool returnValue;
    char repetedPasswordPlayer[100];
    while (true) {
        echo();
        bool repetedPasswordHandle = false;
        erase();
        printw("Press ENTER to validate an entry\nPress ENTER with an empty entry to go back\n");
        refresh();
        printw("\nEnter the username : ");
        char namePlayer[100];
        wgetnstr(stdscr,namePlayer,99);
        if(strlen(namePlayer)==0) break;
        printw("\nEnter the password : ");
        while (true) {
            move(5,21);
            clrtobot();
            noecho();
            char passwordPlayer[100];
            wgetnstr(stdscr,passwordPlayer,99);
            if(strlen(passwordPlayer)==0) break;
            while(strcmp(passwordPlayer, repetedPasswordPlayer)!=0){
                memset(repetedPasswordPlayer, 0, sizeof(repetedPasswordPlayer)); //on vide le buffer pour pas que ca s'accumule
                if(repetedPasswordHandle) printw("\nThe passwords entered do not match !");
                else printw("\nRepeat the password : ");
                move(7,22);
                wgetnstr(stdscr,repetedPasswordPlayer,99);
                if(strlen(repetedPasswordPlayer)==0){repetedPasswordHandle = false;memset(repetedPasswordPlayer, 0, sizeof(repetedPasswordPlayer)); break;}
                repetedPasswordHandle = true;
            }
            if(udm.newAccountCreated(namePlayer, passwordPlayer)){
                napms(1000);
                printw("\nRegistered !\n");
                if(udm.isCorrectLogin(namePlayer, passwordPlayer)){
                    user = namePlayer;
                    connexionData.push_back(namePlayer);
                    connexionData.push_back(passwordPlayer);
                    //udm.setReceivingFileDescriptor(user, socket);
                    printw("You are now Connected in your new account, good game! \n");
                    napms(1000);
                    return connexionData;
                }
                else printw("Error while logging into your account, please try again \n");
                refresh();
                napms(1000);
            }
            else {repetedPasswordHandle = false; memset(repetedPasswordPlayer, 0, sizeof(repetedPasswordPlayer));}
        }
    }
    return connexionData;
}

vector<std::string> InterfaceTerminal::handleConnexionOption() {
    initNcurses();
    vector<std::string> dataConnexion;
    int selectedOption = 0;

    while (true) {
        displayConnexionOption(selectedOption);
        int ch = getch();    //récupère l'entrée utilisateur
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
            case '\n':
                if (selectedOption == 0) { //On a cliqué sur se connecter
                    dataConnexion = handleLogin();
                    if(dataConnexion.size()==2)return dataConnexion;
                    noecho();
                    curs_set(0);
                    nodelay(stdscr, true);
                }
                else if (selectedOption == 1) { //on a cliqué sur register
                    dataConnexion = handleRegister();
                    if(dataConnexion.size()==2)return dataConnexion;
                    noecho();
                    curs_set(0);
                    nodelay(stdscr, true);
                }
            break;
            case 'q':
            case 'Q':

                endwin();
            return dataConnexion;
        }
    }
    return dataConnexion;
}



/**
 *@brief Gère la sélection des options dans le menu principal.
 *@param selectedOption Indice de l'action sélectionnée.
 */
int InterfaceTerminal::handleOptionSelection(int &selectedOption) {
   switch (selectedOption) {
       case 0:
            // Si l'utilisateur sélectionne "Play", on affiche le menu des modes de jeu.
           handleGameModeSelection(selectedOption);
           break;

       case 1:
           // Si l'utilisateur sélectionne "Friends", on affiche les options possibles avec les amis.
           if(handleFriends()<0)return -1;
           break;

       case 2:
           // Si l'utilisateur sélectionne "Account", on affiche les options possibles pour se connecter.
           //handleConnexionOption();
       break;

   }
   return 0;
}


int InterfaceTerminal::handleFriendsOptionSelection(const string friendName) {
    int selectedOption = 0;

    while (true) {
        displayFriendOptions(selectedOption);

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
        case 't':
        case 'T':
           if(!(user==""))if(handleMessageBox()<0)return -1;
           break;
        case 'q':
        case 'Q':
               endwin();
            return 0;
        case '\n':
        switch (selectedOption) {
        case 0:
            // Si l'utilisateur sélectionne "Remove Player", on retire l'amis de la liste d'amis.
            db.deleteFriend(user, friendName);
            erase();
            printw("Friend removed !\n");
            refresh();
            napms(700);
            return 0;
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
int InterfaceTerminal::start() {
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
               if (selectedOption < 2) {
                   selectedOption++;
               }
               break;
           case '\n':   // touche 'entrée'
               handleOptionSelection(selectedOption);
               break;
           case 't':
           case 'T':
               if(!(user==""))if(handleMessageBox()<0)return -1;
               break;
           case 'q':
           case 'Q':
               if(user!=""){int id = udm.getUserId(user.c_str()); udm.logOut(id);}
               endwin();
               return -1;
       }
   }
}
