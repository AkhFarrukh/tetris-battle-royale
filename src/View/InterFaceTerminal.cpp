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
                                            "Royal Competition Mode "  
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
   init_pair(9, COLOR_BLACK, COLOR_WHITE);  // Texte noir sur fond blanc

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
    
    const char* menuOptions[] = { "Play", "Handle friends", "Leaderboard (Endless Mode)" };
    int numberPossibleOptions = std::size(menuOptions);  //  nombre d'options du menu
    
    int width = COLS;   // largeur et hauteur de la windows (via ncurses)
    int height = LINES;
    int menuWidth = 40;  // largeur menu principal
    int menuHeight = 10; // hauteur menu

    // position de depart du menu pour un bon cadrage
    int startX = (width - menuWidth) / 2;
    int startY = (height - menuHeight) / 2;

    const char* title = "Welcome to Tetris Royale (Terminal Edition)";
    int titleLength = strlen(title);

    attron(A_BOLD); // mettre en gras (via ncurses)
    mvprintw(startY - 8, (width - titleLength) / 2, "%s", title);  // titre Afficher au-dessus du cadre
    attroff(A_BOLD);

    // creation d'une fenetre pour le menu ( avec un cadre/bordure)
    WINDOW* menuWindow = newwin(menuHeight, menuWidth, startY, startX);
    box(menuWindow, 0, 0);  // dessine le cadre

    // Display menu options
    for (int i = 0; i < numberPossibleOptions; ++i) {
        if (i == selectedOption) {
            // met en evidence l'option selectionnée
            wattron(menuWindow, COLOR_PAIR(9));   // pour activer  mise en blanc de l'option
            mvwprintw(menuWindow, 3 + i * 2, 2, "-> %d. %s ", i + 1, menuOptions[i]);    // ' * 2 '  pour un avoir espacer un peu plus entre options
            wattroff(menuWindow, COLOR_PAIR(9)); 
        } else {
            mvwprintw(menuWindow, 3 + i*2 , 2, "   %d. %s ", i + 1, menuOptions[i]);
        }
    }
    wrefresh(menuWindow); // Refresh la window

    mvprintw(startY + menuHeight+ 3, (width - 35) / 2, "Use UP and DOWN arrow keys to navigate");           // TODO -- gerer les nombres magiques présents (qui permettent le bon cadrage des elements)
    mvprintw(startY + menuHeight + 4, (width - 35) / 2, "Press ENTER to select an action");
    mvprintw(startY + menuHeight + 5, (width - 35) / 2, "Press 'q' to quit");
    
    refresh();
    
    delwin(menuWindow); // detruire pour libererr mememoire
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
        std::strncpy(messageToSent.expediteur, user, sizeof(messageToSent.expediteur) - 1);
        std::strncpy(messageToSent.message, buffer, sizeof(messageToSent.message) - 1);
        if(!sendAll(socket, &messageSent, sizeof(int))){flag=true; break;}
        if(!sendAll(socket, &size_, sizeof(int))){flag=true; break;}
        sendAll(socket, &messageToSent, sizeof(Message));
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
    //erase();
    int width = COLS;   // largeur et hauteur de la windows (via ncurses)
    int height = LINES;
    int menuWidth = 50;  // largeur menu principal
    int menuHeight = 18; // hauteur menu

    // position de depart du menu pour un bon cadrage
    int startX = (width - menuWidth) / 2;
    int startY = (height - menuHeight) / 2;

    const char* instruction = "Select a Game Mode:";
    int instructionLength = strlen(instruction);

    attron(COLOR_PAIR(2) | A_BOLD); //  mettre en vert +  gras (via ncurses) 
    mvprintw(startY - 5, startX , "%s", instruction);  // titre Afficher au-dessus du cadre
    attroff(COLOR_PAIR(2) | A_BOLD);

    WINDOW* gameModeWindow = newwin(menuHeight, menuWidth, startY, startX); // fenetre pour afficher les modes de jeu
    box(gameModeWindow, 0, 0);  // dessine le cadre

    int numberModePossible = std::size(modeNames);   // taille du tableau contenant les modes de jeu

    for (int i = 0; i < numberModePossible; ++i) {
        if (i == selectedMode) {
            wattron(gameModeWindow, COLOR_PAIR(9));
            mvwprintw(gameModeWindow, 3 + i * 3, 2,"-> %d. %s", i + 1, modeNames[i] ) ;
            wattroff( gameModeWindow, (COLOR_PAIR(9)) );
        } else {
            //printw("   %d. %s", i + 1, modeNames[i]);
            mvwprintw(gameModeWindow, 3 + i * 3, 2, "   %d. %s", i + 1, modeNames[i]);
        }
    }
    wrefresh(gameModeWindow);
    
    //printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select a mode\nPress 'q' to quit\n");
    mvprintw(startY + menuHeight+ 3, (width - 45) / 2, "Use UP and DOWN arrow keys to navigate");           // TODO -- gerer les nombres magiques présents (qui permettent le bon cadrage des elements)
    mvprintw(startY + menuHeight + 4, (width - 45) / 2, "Press ENTER to select an action");
    mvprintw(startY + menuHeight + 5, (width - 45) / 2, "Press 'q' to quit");
    
    if (!(user == "")) mvprintw(startY + menuHeight + 7, (width - 45) / 2, "Press 't' to open the message box");
    //if(!(user==""))printw("Press 't' to open the message box\n");
    
    refresh();
    delwin(gameModeWindow);

}






void InterfaceTerminal::displayMultiplayerOption(int selectedMode) const {
    int action = selectedMode;
    erase();
    printw("\n\nNow pick your action:\n");
    const char* subActions[] = { "Create a room", "Join a room"};
    for (int j = 0; j < 2; ++j) {
        if (j == action) {
            attron(COLOR_PAIR(9));
            printw("-> %s\n", subActions[j]);
            attroff(COLOR_PAIR(9));
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
    //erase();
    
    int width = COLS;   // Largeur et hauteur de la fenêtre (via ncurses)
    int height = LINES;
    int menuWidth = 50;  // Largeur du menu des amis
    int menuHeight = 25; // Hauteur du menu

    // Position pour centrer le cadre
    int startX = (width - menuWidth) / 2;
    int startY = (height - menuHeight - 3) / 2 ;

    const char* title = "Friends list:";
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(startY - 2, startX, "%s", title);  // Titre aligné avec le cadre
    attroff(COLOR_PAIR(2) | A_BOLD);

    // fenêtre pour le menu des amis
    WINDOW* friendsWindow = newwin(menuHeight, menuWidth, startY, startX);
    box(friendsWindow, 0, 0);  // Dessiner le cadre

    int numberFriends = friends.size();  // Nombre d'amis

    // pour afficher les amis
    if (numberFriends == 0) {
        mvwprintw(friendsWindow, 2, 2, "Empty friends list");
    } else {
        for (int i = 0; i < numberFriends; ++i) {
            if (i == selectedOption) {
                wattron(friendsWindow, COLOR_PAIR(9));
                mvwprintw(friendsWindow, 2 + i, 2, "-> %d. %s", i + 1, friends[i].c_str());
                wattroff(friendsWindow, COLOR_PAIR(9));
            } else {
                mvwprintw(friendsWindow, 2 + i, 2, "   %d. %s", i + 1, friends[i].c_str());
            }
        }
    }

    // pour ajouter des options en bas du cadre  (car si il y a trop d'amis --> cadre trop petit)
    int bottomY = menuHeight - 4;
    if (selectedOption == numberFriends) {
        wattron(friendsWindow, COLOR_PAIR(9));
        mvwprintw(friendsWindow, bottomY, 2, "-> Add new friend");
        wattroff(friendsWindow, COLOR_PAIR(9));
    }
    else {
        mvwprintw(friendsWindow, bottomY, 2, "   Add new friend");
    }

    if (selectedOption == numberFriends + 1) {
        wattron(friendsWindow, COLOR_PAIR(9));
        mvwprintw(friendsWindow, bottomY + 2, 2, "-> Friend requests (%d)", db.getNumberFriendRequestsReceived(user));
        wattroff(friendsWindow, COLOR_PAIR(9));
    }
    else {
        mvwprintw(friendsWindow, bottomY + 2, 2, "   Friend requests (%d)", db.getNumberFriendRequestsReceived(user));
    }
    wrefresh(friendsWindow);

    // Instructions sous le cadre
    mvprintw(startY + menuHeight + 2, (width - 45) / 2, "Use UP and DOWN arrow keys to navigate");
    mvprintw(startY + menuHeight + 3, (width - 45) / 2, "Press ENTER to select an option");
    mvprintw(startY + menuHeight + 4, (width - 45) / 2, "Press 'q' to quit");

    if (!(user == "")) 
        mvprintw(startY + menuHeight + 6, (width - 45) / 2, "Press 't' to open the message box");         

    refresh();
    delwin(friendsWindow);
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



void InterfaceTerminal::enterName() {
    erase();  // Nettoyer tout l'écran AVANT d'afficher le cadre

    int width = COLS;
    int height = LINES;
    int menuWidth = 50;
    int menuHeight = 5;
    int startX = (width - menuWidth) / 2;
    int startY = (height - menuHeight) / 2;

    refresh();
    echo();
    curs_set(1);
    nodelay(stdscr, false);

    char namePlayer[100];

    while (!flag) {
        //erase();  // Nettoyer tout l'affichage à chaque itération

        // Affichage des instructions
        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(startY - 4, startX, "Press ENTER to research the player name");
        mvprintw(startY - 3, startX, "Press 'ENTER' with an empty entry to quit");
        attroff(COLOR_PAIR(2) | A_BOLD);

        // Création du cadre pour entrer le nom
        WINDOW* nameWindow = newwin(menuHeight, menuWidth, startY, startX);
        box(nameWindow, 0, 0);
        wattron(nameWindow, COLOR_PAIR(2) | A_BOLD);
        mvwprintw(nameWindow, 0, 2, " ENTER NAME ");
        wattroff(nameWindow, COLOR_PAIR(2) | A_BOLD);
        wrefresh(nameWindow);

        // Saisie utilisateur
        mvwgetnstr(nameWindow, 2, 2, namePlayer, 99);
        delwin(nameWindow);
        refresh();

        // Si l'entrée est vide, quitter et réafficher le menu des amis
        if (strlen(namePlayer) == 0) {
            erase();  // Effacer proprement avant de retourner au menu
            return;
        }

        // Effacer avant d'afficher le message suivant
        erase();

        // Vérifier si le nom est celui de l'utilisateur
        if (strcmp(namePlayer, user.c_str()) == 0) {
            attron(COLOR_PAIR(1) | A_BOLD);
            mvprintw(startY + 6, startX, "Please enter a name different from yours!");
            attroff(COLOR_PAIR(1) | A_BOLD);
            refresh();
            napms(1500);
            continue;
        }

        // Afficher la recherche en cours
        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(startY + 6, startX, "Researching player...");
        attroff(COLOR_PAIR(2) | A_BOLD);
        refresh();
        napms(1000);

        // Vérifier si la demande d'ami est possible
        int returnValue = db.verifyRequestFriend(user, namePlayer);
        erase();  // Efface tout avant d'afficher le résultat

        // Afficher le résultat de la recherche
        if (returnValue == 0) {
            if (db.requestFriend(user, namePlayer)) {
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(startY + 6, startX, "Friend request sent!");
                attroff(COLOR_PAIR(2) | A_BOLD);
            }
        } else if (returnValue == 2) {
            attron(COLOR_PAIR(1) | A_BOLD);
            mvprintw(startY + 6, startX, "%s is already your friend", namePlayer);
            attroff(COLOR_PAIR(1) | A_BOLD);
        } else if (returnValue == 3) {
            attron(COLOR_PAIR(1) | A_BOLD);
            mvprintw(startY + 6, startX, "You have already sent a request to %s", namePlayer);
            attroff(COLOR_PAIR(1) | A_BOLD);
        } else if (returnValue == 4) {
            db.deleteRequestFriend(user, namePlayer);
            db.addFriend(user, namePlayer);
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(startY + 6, startX, "%s already requested you, now you're friends!", namePlayer);
            attroff(COLOR_PAIR(2) | A_BOLD);
        } else {
            attron(COLOR_PAIR(1) | A_BOLD);
            mvprintw(startY + 6, startX, "Invalid username!");
            attroff(COLOR_PAIR(1) | A_BOLD);
        }

        refresh();
        napms(1500);
        erase();
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
/** 
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
*/

 
 void InterfaceTerminal::displayFriendRequests(int selectedOption, vector<string> requestsReceived, vector<string> requestsSent) {
    //erase();
    int width = 80;
    int height = 20;
    int startX = (COLS - width)/ 2;
    int startY = (LINES - height - 10) / 2;

    // Créer une fenêtre avec un grand cadre
    WINDOW* requestWindow = newwin(height, width, startY, startX);
    box(requestWindow, 0, 0);

    // Titre "Requests received" en vert
    wattron(requestWindow, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(requestWindow, 1, 2, "Requests received:");
    wattroff(requestWindow, COLOR_PAIR(2) | A_BOLD);

    int numberRequestsReceived = requestsReceived.size();
    int numberRequestsSent = requestsSent.size();
    
    if (numberRequestsReceived == 0) {
        mvwprintw(requestWindow, 3, 4, "No request received");
    } else {
        for (int i = 0; i < numberRequestsReceived; ++i) {
            if (i == selectedOption) {
                wattron(requestWindow, A_REVERSE);
                mvwprintw(requestWindow, 3 + i, 4, "-> %s wants to be your friend!", requestsReceived[i].c_str());
                wattroff(requestWindow, A_REVERSE);
            } else {
                mvwprintw(requestWindow, 3 + i, 4, "   %s wants to be your friend!", requestsReceived[i].c_str());
            }
        }
    }

    // Titre "Requests sent" en vert
    wattron(requestWindow, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(requestWindow, 10, 2, "Requests sent:");
    wattroff(requestWindow, COLOR_PAIR(2) | A_BOLD);

    if (numberRequestsSent == 0) {
        mvwprintw(requestWindow, 12, 4, "No request sent");
    } else {
        for (int i = 0; i < numberRequestsSent; ++i) {
            if (i + numberRequestsReceived == selectedOption) {
                wattron(requestWindow, A_REVERSE);
                mvwprintw(requestWindow, 12 + i, 4, "-> Friend request sent to %s", requestsSent[i].c_str());
                wattroff(requestWindow, A_REVERSE);
            } else {
                mvwprintw(requestWindow, 12 + i, 4, "   Friend request sent to %s", requestsSent[i].c_str());
            }
        }
    }

    wrefresh(requestWindow);
    
    //Instructions sous le cadre
    mvprintw(startY + height + 2, (COLS - 50) / 2, "Use UP and DOWN arrow keys to navigate");
    mvprintw(startY + height + 3, (COLS - 50) / 2, "Press 'A' to accept the request");
    mvprintw(startY + height + 4, (COLS - 50) / 2, "Press 'R' to refuse");
    mvprintw(startY + height + 5, (COLS - 50) / 2, "Press 'C' to cancel a sent friend request");
    mvprintw(startY + height + 6, (COLS - 50) / 2, "Press 'q' to quit");
    if (!(user == ""))
        mvprintw(startY + height + 7, (COLS - 50) / 2, "Press 't' to open the message box");
    

    refresh();
    delwin(requestWindow);
    //erase();

    
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
                    //refresh(); 
                    
                    handleFriendRequests();
                    
                    // rajouté erase et refresh car l'ecran des requests ne voulait pas se supprimer
                    erase();      
                    refresh();
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
    //erase();
    int width = COLS;   // largeur et hauteur de la windows (via ncurses)
    int height = LINES;
    int menuWidth = 60;  // largeur menu principal
    int menuHeight = 12; // hauteur menu

    // position de depart du menu pour un bon cadrage
    int startX = (width - menuWidth) / 2;
    int startY = (height - menuHeight) / 2;

    WINDOW* connexionOptions_window = newwin(menuHeight, menuWidth, startY, startX);
    box(connexionOptions_window, 0, 0);  // Dessiner le cadre

    // Affichage du titre "TETRIS ROYAL" centré au-dessus du menu
    const char* title = "----- TETRIS ROYAL -----";
    int title_Xpos = (width - strlen(title) - 1) / 2;  // Calculer la position pour centrer le titre
    int title_Ypos = startY - 10;
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(title_Ypos, title_Xpos, title);  // Afficher le titre à la position calculée
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    const char* connexionOptions[] = {"Log in", "Register",};
    
    mvwprintw(connexionOptions_window, 1, 2, "Log in or Register here:");
    
    for (int i = 0; i < std::size(connexionOptions); ++i) {
        
        if (i == selectedOption) {
            wattron(connexionOptions_window, COLOR_PAIR(9));
            mvwprintw(connexionOptions_window, 5 + i*3, 2 , "-> %s",  connexionOptions[i]);
            wattroff(connexionOptions_window, COLOR_PAIR(9));
        } else {
            mvwprintw(connexionOptions_window, 5 + i * 3, 2, "   %s",  connexionOptions[i]);
        }
    }

    wrefresh(connexionOptions_window);
    //printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select an option\nPress 'q' to quit\n");
    //if(!(user==""))printw("Press 't' to open the message box\n");

    mvprintw(startY + menuHeight+ 3, (width - 45) / 2, "Use UP and DOWN arrow keys to navigate");           // TODO -- gerer les nombres magiques présents (qui permettent le bon cadrage des elements)
    mvprintw(startY + menuHeight + 4, (width - 45) / 2, "Press ENTER to select an action");
    mvprintw(startY + menuHeight + 5, (width - 45) / 2, "Press 'q' to quit");


    if (!(user == "")) mvprintw(startY + menuHeight + 7, (width - 35) / 2, "Press 't' to open the message box");
    refresh();

    delwin(connexionOptions_window);
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
 vector<std::string> InterfaceTerminal::handleLogin() {
    int sock = getSock();  // Récupérer le socket de communication avec le serveur
    vector<std::string> connexionData;
    curs_set(1);  // Afficher le curseur
    nodelay(stdscr, false);  // Désactiver le mode non-bloquant
    bool firstTry = true;

    while (!flag) {
        erase();  
        echo();
        clrtobot();

        printw("Press ENTER to validate an entry\nPress ENTER with an empty entry to go back\n");
        refresh();

        int startX = (COLS - 40) / 2;
        int startY = (LINES - 12) / 2 - 5;

        // Fenêtre pour le nom d'utilisateur
        WINDOW* usernameWindow = newwin(5, 40, startY, startX);
        box(usernameWindow, 0, 0);
        wattron(usernameWindow, COLOR_PAIR(2) | A_BOLD);
        mvwprintw(usernameWindow, 1, 2, "Username: ");
        wattroff(usernameWindow, COLOR_PAIR(2) | A_BOLD);
        wrefresh(usernameWindow);

        move(startY + 1, startX + 11);
        char namePlayer[100];
        wgetnstr(usernameWindow, namePlayer, 99);

        if (strlen(namePlayer) == 0) {
            delwin(usernameWindow);
            return connexionData;
        }
        delwin(usernameWindow);

        while (!flag) {
            // Fenêtre pour le mot de passe
            WINDOW* passwordWindow = newwin(5, 40, startY + 8, startX);
            box(passwordWindow, 0, 0);
            wattron(passwordWindow, COLOR_PAIR(2) | A_BOLD);
            mvwprintw(passwordWindow, 1, 2, "Password: ");
            wattroff(passwordWindow, COLOR_PAIR(2) | A_BOLD);
            wrefresh(passwordWindow);

            wmove(passwordWindow, 1, 11);
            noecho();
            char passwordPlayer[100];
            wgetnstr(passwordWindow, passwordPlayer, 99);

            if (strlen(passwordPlayer) == 0) {
                firstTry = true;
                delwin(passwordWindow);
                break;  // Retour à la saisie du nom d'utilisateur
            }
            firstTry = false;

            // --- COMMUNICATION AVEC LE SERVEUR ---
            int type = MessageType::T_Connexion;
            int size = sizeof(struct Connexion);
            Connexion connexion;

            std::strncpy(connexion.username, namePlayer, sizeof(connexion.username) - 1);
            std::strncpy(connexion.password, passwordPlayer, sizeof(connexion.password) - 1);
            sendAll(sock, &type, sizeof(int));
            sendAll(sock, &size, sizeof(int));
            sendAll(sock, &connexion, sizeof(Connexion));

            int type_; int size_;
            if (!recvAll(sock, &type_, sizeof(int)) || !recvAll(sock, &size_, sizeof(int))) {
                flag = true;
                break;
            }
            if (type_ == MessageType::T_ForcedExit) {
                flag = true;
                break;
            }
            bool logResult = (type_ == MessageType::T_AcceptConnexion);

            if (logResult) {
                int doubleSession;
                if (!recvAll(sock, &doubleSession, sizeof(int)) || !recvAll(sock, &size_, sizeof(int))) {
                    flag = true;
                    break;
                }
                bool doubleSessionResult = (doubleSession == MessageType::T_RefuseConnexion);

                if (doubleSessionResult) {
                    // Affichage du message "Already logged in another session"
                    printw("\nYou are already logged in another session! \n");
                    move(startY + 15, startX + 2);
                } else {
                    napms(1000);
                    connexionData.push_back(namePlayer);
                    connexionData.push_back(passwordPlayer);
                    user = namePlayer;

                    move(startY + 15, startX + 2);
                    attron(COLOR_PAIR(2) | A_BOLD);
                    printw("Connected!");
                    attroff(COLOR_PAIR(2) | A_BOLD);
                    wrefresh(passwordWindow);
                    refresh();
                    napms(1000);

                    // Effacer la page et afficher le menu
                    erase();
                    displayMenu(0);
                    refresh();
                    delwin(passwordWindow);
                    return connexionData;
                }
            } else {
                // Affichage du message d'erreur
                move(startY + 15, startX + 2);
                attron(COLOR_PAIR(1) | A_BOLD);
                printw("Incorrect username or password.");
                attroff(COLOR_PAIR(1) | A_BOLD);
                refresh();
                napms(1500);

                move(startY + 15, startX);
                clrtoeol();
                refresh();
                curs_set(1);
                wmove(passwordWindow, 1, 11);
                wrefresh(passwordWindow);
            }
            delwin(passwordWindow);
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
 vector<std::string> InterfaceTerminal::handleRegister() {
    int sock = getSock();  // Récupérer le socket de communication avec le serveur
    vector<std::string> connexionData;
    curs_set(1);  // Afficher le curseur
    nodelay(stdscr, false);  // Désactiver le mode non-bloquant
    bool repetedPasswordHandle = false;
    char repetedPasswordPlayer[100];
    
    while (!flag) {  // Boucle principale qui continue tant que 'flag' est faux
        echo();
        erase();
        printw("Press ENTER to validate an entry\nPress ENTER with an empty entry to go back\n");
        refresh();

        int startX = (COLS - 40) / 2;  // Centrer horizontalement
        int startY = (LINES - 12) / 2 - 5; // Centrer verticalement

        // Fenêtre pour le nom d'utilisateur
        WINDOW* usernameWindow = newwin(5, 40, startY, startX);
        box(usernameWindow, 0, 0);
        wattron(usernameWindow, COLOR_PAIR(2) | A_BOLD);
        mvwprintw(usernameWindow, 1, 2, "Username: ");
        wattroff(usernameWindow, COLOR_PAIR(2) | A_BOLD);
        wrefresh(usernameWindow);

        char namePlayer[100];
        mvwgetnstr(usernameWindow, 1, 11, namePlayer, 99);
        if (strlen(namePlayer) == 0) {
            delwin(usernameWindow);
            return connexionData;
        }
        delwin(usernameWindow);

        while (!flag) {
            // Fenêtre pour le mot de passe
            WINDOW* passwordWindow = newwin(5, 40, startY + 8, startX);
            box(passwordWindow, 0, 0);
            wattron(passwordWindow, COLOR_PAIR(2) | A_BOLD);
            mvwprintw(passwordWindow, 1, 2, "Password: ");
            wattroff(passwordWindow, COLOR_PAIR(2) | A_BOLD);
            wrefresh(passwordWindow);

            noecho();
            char passwordPlayer[100];
            memset(passwordPlayer, 0, sizeof(passwordPlayer));
            mvwgetnstr(passwordWindow, 1, 11, passwordPlayer, 99);
            if (strlen(passwordPlayer) == 0) {
                delwin(passwordWindow);
                break;
            }
            delwin(passwordWindow);

            while (!flag) {
                // Fenêtre pour répéter le mot de passe
                WINDOW* repeatPassword_window = newwin(5, 40, startY + 16, startX);
                box(repeatPassword_window, 0, 0);
                wattron(repeatPassword_window, COLOR_PAIR(2) | A_BOLD);
                mvwprintw(repeatPassword_window, 1, 2, "Repeat password: ");
                wattroff(repeatPassword_window, COLOR_PAIR(2) | A_BOLD);
                wrefresh(repeatPassword_window);

                memset(repetedPasswordPlayer, 0, sizeof(repetedPasswordPlayer));
                mvwgetnstr(repeatPassword_window, 1, 18, repetedPasswordPlayer, 99);

                if (strlen(repetedPasswordPlayer) == 0) {
                    delwin(repeatPassword_window);
                    break;
                }

                // Vérification si les mots de passe correspondent
                if (strcmp(passwordPlayer, repetedPasswordPlayer) != 0) {
                    wattron(repeatPassword_window, COLOR_PAIR(1));
                    mvwprintw(repeatPassword_window, 2, 2, "Passwords do not match!");
                    wattroff(repeatPassword_window, COLOR_PAIR(1));
                    wrefresh(repeatPassword_window);
                    napms(1000);
                    delwin(repeatPassword_window);
                    refresh();
                } else {
                    delwin(repeatPassword_window);
                    refresh();
                    break;
                }
            }


            // --- ENVOI DES INFORMATIONS AU SERVEUR ---
            int typeRegister = MessageType::T_Register;
            int size = sizeof(struct Connexion);
            Connexion connexion;
            std::strncpy(connexion.username, namePlayer, sizeof(connexion.username) - 1);
            std::strncpy(connexion.password, passwordPlayer, sizeof(connexion.password) - 1);

            sendAll(sock, &typeRegister, sizeof(int));
            sendAll(sock, &size, sizeof(int));
            sendAll(sock, &connexion, sizeof(Connexion));

            int typeResult_; int size_;
            if (!recvAll(sock, &typeResult_, sizeof(int)) || !recvAll(sock, &size_, sizeof(int))) {
                flag = true;
                break;
            }

            if (typeResult_ == MessageType::T_ForcedExit) {
                flag = true;
                break;
            }

            bool registerResult = (typeResult_ == MessageType::T_AcceptConnexion);

            if (registerResult) {
                napms(1000);
                erase();
                printw("\nRegistered!\n");

                // Tentative de connexion après enregistrement
                int typeLog_; int size;
                if (!recvAll(sock, &typeLog_, sizeof(int)) || !recvAll(sock, &size, sizeof(int))) {
                    flag = true;
                    break;
                }

                if (typeLog_ == MessageType::T_AcceptConnexion) {
                    user = namePlayer;
                    connexionData.push_back(namePlayer);
                    connexionData.push_back(passwordPlayer);

                    attron(COLOR_PAIR(2) | A_BOLD);
                    printw("You are now connected. Good game!\n");
                    attroff(COLOR_PAIR(2) | A_BOLD);
                    refresh();
                    napms(1000);

                    erase();
                    displayMenu(0);
                    refresh();
                    return connexionData;
                } else {
                    attron(COLOR_PAIR(1) | A_BOLD);
                    printw("Error while logging into your account. Please try again.\n");
                    attroff(COLOR_PAIR(1) | A_BOLD);
                }

                refresh();
                napms(1000);
            } else {
                repetedPasswordHandle = false;
                memset(repetedPasswordPlayer, 0, sizeof(repetedPasswordPlayer));

                move(startY + 20, startX);
                attron(COLOR_PAIR(1) | A_BOLD);
                printw("Username and/or password invalid!\n");
                attroff(COLOR_PAIR(1) | A_BOLD);
                refresh();
                napms(1500);
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
        erase();
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
               erase();
               refresh();
                returnValue = handleOptionSelection(selectedOption);

                erase();
                refresh();
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
