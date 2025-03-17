#include "InterFaceTerminal.hpp"
#include <ncurses.h>
#include <locale.h>  // Needed for wide characters (if printing Unicode blocks)



Database db("tetris.db");
UserDatabaseManager udm;
Chat chat;

atomic<bool> flag = false; //informer thread de se terminer (ctrlC notamment)
std::mutex interfaceMutex;
std::string user = "";

const char* InterfaceTerminal::modeNames[] = {
                                            "Endless Mode",  
                                            "Classic Mode",  
                                            "Duel Mode",  
                                            "Royal Competition Mode (Unreleased)"  
                                            };    

InterfaceTerminal::InterfaceTerminal() {initNcurses();}
InterfaceTerminal::~InterfaceTerminal() {}


/**
* @brief Initialise ncurses et configure l'affichage du terminal.
*/
void InterfaceTerminal::initNcurses() {
   setlocale(LC_ALL, ""); //Enable UTF-8
   initscr();
   cbreak();
   noecho();
   keypad(stdscr, TRUE);
   curs_set(0);
   nodelay(stdscr, TRUE);
   start_color();
   init_pair(8, COLOR_BLACK, COLOR_WHITE);  // Texte noir sur fond blanc

   if (has_colors() && can_change_color()) {
       use_default_colors();
   }
   init_pair(1, COLOR_RED, -1);
   init_pair(2, COLOR_GREEN, -1);
   init_pair(3, COLOR_YELLOW, -1);
   init_pair(4, COLOR_BLUE, -1);
   init_pair(5, COLOR_MAGENTA, -1);
   init_pair(6, COLOR_CYAN, -1);
   init_color(8, 500, 0, 500);
   init_pair(7, 8, -1);
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
   
   int numberPossibleOptions = std::size(menuOptions); // taille du tableau contenant options du menu principal
   
   for (int i = 0; i < numberPossibleOptions; ++i) {
       if (i == selectedOption) {
           attron(COLOR_PAIR(8));    // activer couleur noir sur blanc pour option selected
           printw("-> %d. %s\n", i + 1, menuOptions[i]);
           attroff(COLOR_PAIR(8));
       } else {
           printw("   %d. %s\n", i + 1, menuOptions[i]);
       }
   }
   printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select an action\nPress 'q' to quit\n");
   refresh();
}


/**
 * @brief Affiche la boîte de messagerie.
 * 
 * Méthode affiche la liste des amis de l'utilisateur et indique le nombre de messages non lus pour chacun.
 * L'utilisateur peut naviguer dans la liste et sélectionner un ami pour ouvrir une conversation.
 * 
 * @param selectedOption L'option actuellement sélectionnée par l'utilisateur.
 * @param friends Liste des amis de l'utilisateur.
 */

void InterfaceTerminal::displayMessageBox(int selectedOption, vector<string> friends) {
    erase();
    printw("Message box : \n\n");
    int user_id = db.getUserId(user.c_str());
    int numberFriends = std::size(friends);   // taille du tableau contenant les amis
    if(numberFriends == 0) {printw("Empty friends list \n");}
    else{
        for (int i = 0; i < numberFriends; ++i) {
            int friend_id = db.getUserId(friends[i].c_str());
            if (i == selectedOption) {
                printw("-> %d. %s(%d)", i + 1, friends[i].c_str(), db.getNumberMessageNoRead(user_id, friend_id));
            } else {
                printw("   %d. %s(%d)", i + 1, friends[i].c_str(), db.getNumberMessageNoRead(user_id, friend_id));
            }
            printw("\n");
        }
    }

    printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select a friend\nPress 'q' to quit\n");
    refresh();
}



/**
 * @brief Gère la boîte de messagerie de l'utilisateur.
 * 
 * Cette fonction affiche la liste des amis de l'utilisateur et lui permet de sélectionner 
 * un contact pour discuter via une interface ncurses. L'utilisateur peut naviguer dans la liste, 
 * ouvrir une conversation ou quitter l'interface.
 * 
 * @return 0 si l'utilisateur quitte normalement, -1 en cas d'erreur lors du chat.
 */

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
    while (!flag) {

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
            return -1;
        }
    }
    return -1;
}


/**
 * @brief Affiche les messages non lus.
 *
 * Affiche tous les messages non lus envoyés par l'ami `friendName` à l'utilisateur `user`.
 * Si l'affichage dépasse la taille de la fenêtre, un défilement automatique est appliqué.
 * 
 * @param user Nom de l'utilisateur actuel.
 * @param friendName Nom de l'ami dont on affiche les messages non lus.
 */
void InterfaceTerminal::displayMessageNoRead(const std::string& user, const string& friendName){
    int user_id = db.getUserId(user.c_str());
    int friend_id = db.getUserId(friendName.c_str());
    vector<std::string> messagesNoRead = db.getMessages(friend_id, user_id);
    nodelay(stdscr, false);
    printw("Press ENTER to send the message\nPress 'ENTER' with an empty entry to quit\n");
    printw("\nDiscussion with %s : \n", friendName.c_str());
    refresh();
    curs_set(0);
    for (const auto message : messagesNoRead){
    int currentLine = LINES-1;
    while(mvinch(currentLine-1, 0) == ' ' || mvinch(currentLine-1, 0) == ERR){ //tant que ligne est vide
        currentLine--;
    }
    move(currentLine, 0);
    printw("%s : %s\n", friendName.c_str(), message.c_str());
    if (LINES-currentLine<5) {
        move(LINES-1, 0);
        clrtoeol();
        scrollok(stdscr, true);
        scroll(stdscr);
        scrollok(stdscr, false);
        move(3, 0);clrtoeol();
        move(2, 0);clrtoeol();
        move(1, 0);clrtoeol();
        move(0, 0);clrtoeol();
        printw("Press ENTER to send the message\nPress 'ENTER' with an empty entry to quit\n");
        printw("\nDiscussion with %s : \n", friendName.c_str());
    }
    refresh();
    curs_set(1);}
}



/**
 * @brief Permet à l'utilisateur de discuter avec un ami via une interface terminal.
 * 
 * Cette fonction affiche la conversation avec un ami et permet d'envoyer des messages.
 * Les messages sont envoyés immédiatement si l'ami est en ligne et dans la conversation,
 * sinon, ils sont stockés, et on peut y avoir acces par apres.
 * 
 * @param user Nom de l'utilisateur.
 * @param friendName Nom de l'ami avec qui discuter.
 * @return 0 en cas de succès, -1 en cas d'erreur de connexion.
 */

int InterfaceTerminal::chatWith(const std::string& user, const string& friendName){
    int user_id = db.getUserId(user.c_str());
    int friend_id = db.getUserId(friendName.c_str());
    erase();
    interfaceMutex.lock();
    displayMessageNoRead(user, friendName);
    interfaceMutex.unlock();
    db.deleteMessages(friend_id, user_id);
    db.conversationOpen(user_id, friend_id);
    refresh();
    echo();
    curs_set(1);
    nodelay(stdscr, false);
    bool windowFull = false;
    while(!flag){
        if (windowFull) {
            move(3, 0);clrtoeol();
            move(2, 0);clrtoeol();
            move(1, 0);clrtoeol();
            move(0, 0);clrtoeol();
            printw("Press ENTER to send the message\nPress 'ENTER' with an empty entry to quit\n");
            printw("\nDiscussion with %s : \n", friendName.c_str());
        }
        char buffer[1024];
        move(LINES- 1, 0);
        clrtoeol();
        printw("Enter your message : ");
        refresh();
        wgetnstr(stdscr,buffer,1023-friendName.size()-1);
        if(strlen(buffer)==0){db.conversationClose(user_id, friend_id); return 0;}
        else{
        ///
        /// MUTEX DEB
        interfaceMutex.lock();
        refresh();
        curs_set(0);
        int currentLine = LINES-1;
        while(mvinch(currentLine-1, 0) == ' ' || mvinch(currentLine-1, 0) == ERR){ //tant que ligne est vide
            currentLine--;
            }
        move(currentLine, 0);
        printw("%s : %s\n", user.c_str(), buffer);
        if(!(udm.isLoggedIn(friendName.c_str()))){printw("Message not delivered, %s is offline\n", friendName.c_str());
          currentLine++;
          if(LINES-1-currentLine < 5){
              move(LINES-1, 0);
              clrtoeol();
              scrollok(stdscr, true);
              scroll(stdscr);
              scrollok(stdscr, false);
        }
        }
        refresh();
        if(LINES-1-currentLine < 5){ //bcp de msg donc on arrive proche de la ligne de fin
          windowFull = true;
          move(LINES-1, 0);
          clrtoeol();
          scrollok(stdscr, true);
          scroll(stdscr);
          scrollok(stdscr, false);
        }
        refresh();
        curs_set(1);
        interfaceMutex.unlock();
        /// MUTEX FIN
        ///
        int socket = getSock();
        if(udm.isLoggedIn(friendName.c_str())){
          if(db.getInConversation(user_id, friend_id) == 1){ //si le destinataire est sur la conv
        int messageSent = MessageType::T_Message;
        int size_ = sizeof(struct Message);
        Message messageToSent;
      //int bytesSentType = sendAll(socket, &messageSent, sizeof(int));
      //int bytesSentSize = sendAll(socket, &size_, sizeof(int));
       /*d::strncpy(messageToSent.expediteur, user, sizeof(message  e) - 1);
       //essageToSent.destinataire =
      //messageToSent.message =
      //ssize_t bytesWritten = write(socket , message.data() , friendName.size()+1+strlen(buffer)+1);
        if (bytesWritten <= 0){
            if (errno == EPIPE){
                perror("Serveur est déconnecté");}
            else{
                perror("Erreur d'écriture");}
            db.conversationClose(user_id, friend_id);
            return -1;
        }
        else{//memset(buffer, 0, sizeof(buffer));
          }*/
    }
    else{ //si le correspondant n'est pas dans la conv
        db.addMessage(user_id, friend_id, buffer);
    }
}
        }
    }
    db.conversationClose(user_id, friend_id);
    return -1;
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
           attron(COLOR_PAIR(8));   // activer couleur noir sur blanc pour mode selectionné
           printw("-> %d. %s", i + 1, modeNames[i]);
           attroff(COLOR_PAIR(8));
       } else {
           printw("   %d. %s", i + 1, modeNames[i]);
       }
       printw("\n");
   }
   printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select a mode\nPress 'q' to quit\n");
   refresh();
}

void InterfaceTerminal::displayMultiplayerOption(int selectedMode) const {
    int action = selectedMode;
    erase();
    printw("\n\nNow pick your action:\n");
    const char* subActions[] = { "Create a room", "Join a room"};
    for (int j = 0; j < 2; ++j) {
        if (j == action) {
            attron(COLOR_PAIR(8));
            printw("-> %s\n", subActions[j]);
            attroff(COLOR_PAIR(8));
        }
        else {
            printw("   %s\n", subActions[j]);
        }
    }

    printw("\nUse UP/DOWN to select mode, LEFT/RIGHT to pick action.\nPress ENTER to confirm.\nPress 'q' to quit.\n");
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
   while (!flag) {

    displayGameModeMenu(selectedMode);  // affiche le menu avec les modes

    int ch = getch();    // récupère l'entrée utilisateur
       switch (ch) {  
        
        case KEY_UP:     // aller vers le haut
               if (selectedMode > 0) {
                   selectedMode--;
               }
               break;
           case KEY_DOWN: // descendre
               if (selectedMode < static_cast<int>(std::size(modeNames)) - 1) {
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
                   return selectedMode;
                }
                if (selectedMode == 1){
                    erase();
                    printw("Classic Mode selected!\n");
                    refresh();
                    int act = 0;
                    while (!flag){
                        displayMultiplayerOption(act);
                        int ch = getch();
                        switch (ch) {
                            case KEY_UP:
                                if (act > 0) act--;
                                break;
                            case KEY_DOWN:
                                if (act < 2) act++;
                                break;
                            case '\n': {
                                if (act == 0){
                                    return 10;
                                }
                                else {
                                    return 11;
                                }
                            }
                            case 'q':
                            case 'Q':
                                endwin();
                                return -1;
                            case 'ESC':
                                break;
                        }
                    }

                }
                if (selectedMode == 2) {
                    erase();
                    printw("Duel Mode selected!\n");
                    refresh();
                    int act = 0;
                    while (!flag){
                        displayMultiplayerOption(act);
                        int ch = getch();
                        switch (ch) {
                            case KEY_UP:
                                if (act > 0) act--;
                                break;
                            case KEY_DOWN:
                                if (act < 2) act++;
                                break;
                            case '\n': {
                                if (act == 0){
                                    return 20;
                                }
                                else {
                                    return 21;
                                }
                            }
                            case 'q':
                            case 'Q':
                                endwin();
                                return -1;
                            case 'ESC':
                                break;
                        }
                    }

                }
                if (selectedMode == 3) {
                    erase();
                    printw("Royal Competition Mode selected!\n");
                    refresh();
                    int act = 0;
                    while (!flag){
                        displayMultiplayerOption(act);
                        int ch = getch();
                        switch (ch) {
                            case KEY_UP:
                                if (act > 0) act--;
                                break;
                            case KEY_DOWN:
                                if (act < 2) act++;
                                break;
                            case '\n': {
                                if (act == 0){
                                    return 30;
                                }
                                else {
                                    return 31;
                                }
                            }
                            case 'q':
                            case 'Q':
                                endwin();
                                return -1;
                            case 'ESC':
                                break;
                        }
                    }
               }
               break;
           case 't':
           case 'T':
               if(!(user==""))if(handleMessageBox()<0)return -1;
            break;
           case 'q':
           case 'Q':
               endwin();
               return -1;
       }
   }
   return 0;
}



/**
 * @brief Gère l'affichage et l'interaction avec les demandes d'amis.
 * 
 * Cette fonction affiche les demandes d'amis reçues et envoyées, et permet à l'utilisateur 
 * de naviguer dans la liste avec les flèches haut/bas. Il peut accepter, refuser ou annuler 
 * une demande via les touches correspondantes. L'utilisateur peut également ouvrir la boîte 
 * de messagerie ou quitter l'interface.
 * 
 * @return Retourne 0 si l'utilisateur quitte normalement, ou -1 en cas d'erreur lors de l'ouverture.
 */

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
    /* LA METHODE EST DEFAILLANTE */
    //erase();
    //printw("Choose an action to do with :\n\n");
    //int numberFriendsOptionsPossible = 5;   // taille du tableau contenant les modes de jeu
//
    //for (int i = 0; i < numberFriendsOptionsPossible; ++i) {
    //    if (i == selectedOption) {
    //        printw("-> %d. %s", i + 1, friendsOptions[i]);
    //    } else {
    //        printw("   %d. %s", i + 1, friendsOptions[i]);
    //    }
    //    printw("\n");
    //}
    //printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select an option\nPress 'q' to quit\n");
    //if(!(user==""))printw("Press 't' to open the message box\n");
    //refresh();
}


/**
 * @brief Permet à l'utilisateur de rechercher un autre utilisateur en en entrant le username de celui -ci.
 *
 * L'utilisateur entre un nom de joueur. Si le champ est vide, la fonction se termine.
 * Sinon, elle vérifie si l'utilisateur peut envoyer une demande d'ami :
 * - Envoie la demande si possible.
 * - Informe si le joueur est déjà ami.
 * - Informe si une demande a déjà été envoyée.
 * - Si le joueur avait déjà envoyé une demande, la demande d'ami est acceptée.
*/
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

    while (!flag) {
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


/**
 * @brief Affiche les demandes d'amis reçues et envoyées.
 * 
 * affiche les demandes d'amis en attente et permet à l'utilisateur 
 * de naviguer dans la liste avec les flèches haut/bas. Le user peut accepter, 
 * refuser ou annuler une demande via certaines touches.
 * 
 * @param selectedOption Indice de la demande actuellement sélectionnée.
 * @param requestsReceived Vecteur contenant les noms des utilisateurs qui ont envoyé demande d'ami.
 * @param requestsSent Vecteur contenant noms des utilisateurs à qui une demande d'ami a été envoyée.
 */
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


/**
 * @brief Gère l'affichage et l'interaction avec les demandes d'amis.
 * 
 * Cette fonction affiche les demandes d'amis reçues et envoyées, et permet à l'utilisateur 
 * de naviguer dans la liste avec les flèches haut/bas. Il peut accepter, refuser ou annuler 
 * une demande via les touches correspondantes. L'utilisateur peut également ouvrir la boîte 
 * de messagerie ou quitter l'interface.
 * 
 * @return Retourne 0 si l'utilisateur quitte normalement, ou -1 en cas d'erreur lors de l'ouverture.
 */
int InterfaceTerminal::handleFriendRequests() {
    int selectedOption = 0;

    while (!flag) {

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
            return -1;
        }
    }
    return -1;
}


/**
 * @brief Gère l'interface des amis de l'utilisateur.
 * 
 * Affiche la liste des amis de l'utilisateur et lui permet d'interagir avec eux.
 * L'utilisateur peut naviguer dans la liste, sélectionner un ami pour interagir, ajouter un nouvel ami
 * ou consulter ses demandes d'amis reçues.
 * 
 * @return 0 si l'utilisateur quitte normalement, -1 si erreur.
 */
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
    while (!flag) {

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
            return -1;
        }
    }
    return -1;
}

/**
 * @brief Affiche à l'écran les  options de connexion d'utilisateur via le terminal.
 * 
 * Affiche une interface  permettant à un utilisateur 
 * d'entrer un username et un mot de passe pour se connecter au serveur.
 * 
 * @param selectedOption indice de l'option choisie.
 */
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


/**
 * @brief Gère la connexion d'un utilisateur via le terminal.
 * 
 * Affiche une interface  permettant à un utilisateur 
 * d'entrer un username et un mot de passe pour se connecter au serveur.
 * Elle envoie les informations au serveur et attend une réponse pour valider la connexion.
 * 
 * @return Un vecteur contenant le nom d'utilisateur et le mot de passe si la connexion est réussie, sinon un vecteur vide.
 */
vector<std::string> InterfaceTerminal::handleLogin(){
    int sock = getSock();
    vector<std::string> connexionData;
    curs_set(1);
    nodelay(stdscr, false);
    bool returnValue;
    while (!flag) {
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
        while (!flag) {
            if(firstTry) printw("\nEnter your password : ");
            noecho();
            char passwordPlayer[100];
            wgetnstr(stdscr,passwordPlayer,99);
            if(strlen(passwordPlayer)==0){firstTry = true; break;}
            firstTry = false;
            int type = MessageType::T_Connexion;
            int size = sizeof(struct Connexion);
            //demande au serveur de verifier
            Connexion connexion;
            std::strncpy(connexion.username, namePlayer, sizeof(connexion.username) - 1);
            std::strncpy(connexion.password, passwordPlayer, sizeof(connexion.password) - 1);
            int bytesSentType = sendAll(sock, &type, sizeof(int));
            int bytesSentSize = sendAll(sock, &size, sizeof(int));
            sendAll(sock, &connexion, sizeof(Connexion));
            int type_;int size_;
            if(!recvAll(sock, &type_, sizeof(int))){
                flag = true;
                break;
            }
            if(!recvAll(sock, &size_, sizeof(int))){
                flag = true;
                break;
            }
            if (type_ == MessageType::T_ForcedExit){
                flag = true;
                break;
            }
            bool logResult = (type_ == MessageType::T_AcceptConnexion);
            //
            if(logResult){
                int doubleSession;
                if(!recvAll(sock, &doubleSession, sizeof(int))){
                    flag = true;
                    break;
                }
                if(!recvAll(sock, &size_, sizeof(int))){
                    flag = true;
                    break;
                }
                bool doubleSessionResult = (doubleSession == MessageType::T_RefuseConnexion);
                if(doubleSessionResult){printw("\nYou are already logged in another session ! \n"); move(5,22);}
                else{
               		napms(1000);
               		connexionData.push_back(namePlayer);
               		connexionData.push_back(passwordPlayer);
               		user = namePlayer;
               		printw("\nConnected ! \n");
               		refresh();
               		napms(1000);
               		return connexionData;}
            }
            else {printw("\nIncorrect username or password. \n"); move(5,22);}
            }
    }
    return connexionData;
}



/**
 * @brief Gère la connexion d'un utilisateur via le terminal.
 * 
 * Affiche une interface  permettant à un utilisateur 
 * d'entrer un username et un mot de passe pour se connecter au serveur.
 * Elle envoie les informations au serveur et attend une réponse pour valider la connexion.
 * 
 * @return Un vecteur contenant le nom d'utilisateur et le mot de passe si la connexion est réussie, sinon un vecteur vide.
 */
vector<std::string> InterfaceTerminal::handleRegister(){
    int sock = getSock();
    vector<std::string> connexionData;
    curs_set(1);
    nodelay(stdscr, false);
    bool returnValue;
    char repetedPasswordPlayer[100];
    while (!flag) {
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
        while (!flag) {
            noecho();
            move(5,22);
            char passwordPlayer[100];
            wgetnstr(stdscr,passwordPlayer,99);
            if(strlen(passwordPlayer)==0) break;
            while(!flag && strcmp(passwordPlayer, repetedPasswordPlayer)!=0){
                memset(repetedPasswordPlayer, 0, sizeof(repetedPasswordPlayer)); //on vide le buffer pour pas que ca s'accumule
                if(repetedPasswordHandle) printw("\nThe passwords entered do not match !");
                else printw("\nRepeat the password : ");
                move(7,22);
                wgetnstr(stdscr,repetedPasswordPlayer,99);
                if(strlen(repetedPasswordPlayer)==0){clrtoeol(); repetedPasswordHandle = false;memset(repetedPasswordPlayer, 0, sizeof(repetedPasswordPlayer)); break;}
                repetedPasswordHandle = true;
            }
            int typeRegister = MessageType::T_Register;
            int size = sizeof(struct Connexion);
            //demande au serveur de verifier
            Connexion connexion;
            std::strncpy(connexion.username, namePlayer, sizeof(connexion.username) - 1);
            std::strncpy(connexion.password, passwordPlayer, sizeof(connexion.password) - 1);
            int bytesSentType = sendAll(sock, &typeRegister, sizeof(int));
            int bytesSentSize = sendAll(sock, &size, sizeof(int));
            sendAll(sock, &connexion, sizeof(Connexion));
            int typeResult_; int size_;
            if(!recvAll(sock, &typeResult_, sizeof(int))){
                flag = true;
                break;
            }
            if(!recvAll(sock, &size_, sizeof(int))){
                flag = true;
                break;
            }
            if (typeResult_ == MessageType::T_ForcedExit){
                flag = true;
                break;
            }
            bool registerResult = (typeResult_ == MessageType::T_AcceptConnexion);
            if(registerResult){
                napms(1000);
                printw("\nRegistered !\n");
                int typeLog_; int size;
                if(!recvAll(sock, &typeLog_, sizeof(int))){
                	flag = true;
                	break;
            	}
            	if(!recvAll(sock, &size, sizeof(int))){
                	flag = true;
                	break;
            	}
                if(typeLog_ == MessageType::T_AcceptConnexion){
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
            else {repetedPasswordHandle = false; memset(repetedPasswordPlayer, 0, sizeof(repetedPasswordPlayer));
              move(5,21);
              clrtoeol();
              move(9,0);
              printw("username and/or password invalid !\n");

            }
        }
    }
    return connexionData;
}



/**
 * @brief Gère l'interface de connexion et l'inscription de l'utilisateur/joueur.
 *
 * Cette méthode affiche un menu permettant à l'utilisateur de choisir entre 
 * la connexion et l'inscription. Elle capture l'entrée de l'utilisateur et 
 * exécute l'action correspondante. Si l'utilisateur réussit à se connecter 
 * ou à s'inscrire, la méthode retourne un vecteur contenant les informations 
 * de connexion (nom d'utilisateur et mot de passe). 
 * 
 * @return Un vecteur (de string) contenant les informations de connexion 
 *         (nom d'utilisateur et mot de passe) en cas de succès, ou un vecteur vide sinon.
 */
vector<std::string> InterfaceTerminal::handleConnexionOption() {
    //initNcurses();
    vector<std::string> dataConnexion;
    int selectedOption = 0;

    while (!flag) {
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
              int sock = getSock();
              int exitType = MessageType::T_Exit;
              int size = sizeof(int);
              int bytesSentType = sendAll(sock, &exitType, sizeof(int));
              int bytesSentSize = sendAll(sock, &size, sizeof(int));
                endwin();
            return dataConnexion;
        }
    }
    return dataConnexion;
}



/**
 *@brief Gère la sélection des options dans le menu principal.                                            
 *
 *@param selectedOption Indice de l'action sélectionnée.
 */
int InterfaceTerminal::handleOptionSelection(int &selectedOption) {
   switch (selectedOption) {
       case 0:
            // Si l'utilisateur sélectionne "Play", on affiche le menu des modes de jeu.
           return handleGameModeSelection(selectedOption);
           break;

       case 1:
           // Si l'utilisateur sélectionne "Friends", on affiche les options possibles avec les amis.
           handleFriends();
           break;

       case 2:
           // Si l'utilisateur sélectionne "Account", on affiche les options possibles pour se connecter.
           //handleConnexionOption();
       break;

   }
   return -1;
}


/**
 * @brief Gère la sélection des options lié aux "amis" dans la liste d'amis.
 *
 * Cette méthode affiche un menu avec différentes options pour interagir avec un ami,
 * comme le supprimer de la liste d'amis ou lui envoyer un message.
 * Le user parcourt à l'aide des touches directionnelles et valide son choix 
 * avec la touche Entrée.
 *
 * @param friendName Le nom de l'ami sélectionné.
 * @return Retourne 0 si l'opération s'est bien déroulée, ou -1 si erreur
 */
int InterfaceTerminal::handleFriendsOptionSelection(const string friendName) {
    int selectedOption = 0;

    while (!flag) {
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
            return -1;
        case '\n':
        switch (selectedOption) {
        case 0:
            // Si l'utilisateur sélectionne "Remove Player", on retire l'amis de la liste d'amis.
            db.deleteFriend(user, friendName);
            erase();
            printw("Friend removed !\n");
            refresh();
            napms(700);
            return -1;
        break;
        case 1:
            // Si l'utilisateur sélectionne "Send message", on envoie un message à l'ami.
                handleFriends();
        break;
    }
  }
  }
  return -1;
}

/**
 *@brief Démarre l'interface terminal utilisateur et 
 *gère les interactions utilisateur dans
 *le menu principal.
 *
 */
int InterfaceTerminal::start() {
   int returnValue = -1;
   initNcurses();
   int selectedOption = 0;

   while (!flag) {
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
                returnValue = handleOptionSelection(selectedOption);
               if(returnValue!=-1)return returnValue;
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
   return -1;
}
