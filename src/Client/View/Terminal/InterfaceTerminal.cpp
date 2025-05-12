#include "InterfaceTerminal.hpp"
#include <ncurses.h>
#include <locale.h>  // Needed for wide characters (if printing Unicode blocks)


atomic<bool> flag = false; // informer thread de se terminer (Ctrl + C notamment)
std::mutex interfaceMutex;
std::mutex queueMutex;
std::condition_variable cv;
std::queue<std::vector<std::pair<std::string, int>>> messageQueue;
std::queue<std::vector<std::string>> messageVectQueue;
std::queue<int> messageIntQueue;

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


void InterfaceTerminal::display(int selectedOption, Menus menuChoosen, bool& flagChangedMenu, int& number) {
    if (flagChangedMenu){
        erase();
        refresh();
        flagChangedMenu = false;
    }
    switch (menuChoosen)
    {
        case Menus::MainMenu:
        {
            vector<string> menuOptions = { "Welcome to Tetris Royale (Terminal Edition)", "Play", "Handle friends", "Leaderboard (Endless Mode)" };
            Menu menuMain(Menus::MainMenu, menuOptions, COLS, LINES, 40, 10);
            menuMain.displayMenu(selectedOption); // Affiche le menu principal
            break;
        }
        case Menus::GameModeMenu:
        {
            vector<string> menuOptions = { "Select a Game Mode :", "Endless Mode", "Duel Mode", "Classic Mode", "Royal Competition Mode" };
            Menu menuGameMode(Menus::GameModeMenu, menuOptions, COLS, LINES, 50, 18);
            menuGameMode.displayMenu(selectedOption); // Affiche le menu principal
            break;
        }
        case Menus::JoinCreateMenu:
        {
            vector<string> menuOptions = { "Now pick your action:", "Create a room", "Join a room" };
            Menu menuGameMode(Menus::JoinCreateMenu, menuOptions, COLS, LINES, 50, 18);
            menuGameMode.displayMenu(selectedOption); // Affiche le menu principal
            break;
        }
        case Menus::FriendsMenu:
        {
            int type_ = MessageType::T_FriendListRequest;
            int size_ = sizeof(int);
            sendAll(sock_, &type_, sizeof(int));
            sendAll(sock_, &size_, sizeof(int));
            vector<string> friendsList;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, []{ return !messageVectQueue.empty(); });  // Attend un message
                friendsList = messageVectQueue.front();
                messageVectQueue.pop();
            }
            displayFriendsMenu(selectedOption, friendsList); // Affiche le menu des amis
            number = friendsList.size()+2;
            break;
        }
        case Menus::FriendRequestMenu:
        {
            vector<string> menuOptions = { "Friend Requests", "Received requests", "Sent requests" };
            int type = MessageType::T_FriendListReceivedRequest;
            int size = sizeof(int);
            sendAll(sock_, &type, sizeof(int));
            sendAll(sock_, &size, sizeof(int));
            vector<string> requestsReceived;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, []{ return !messageVectQueue.empty(); });  // Attend un message
                requestsReceived = messageVectQueue.front();
                messageVectQueue.pop();
            }
            int type_ = MessageType::T_FriendListSentRequest;
            int size_ = sizeof(int);
            sendAll(sock_, &type_, sizeof(int));
            sendAll(sock_, &size_, sizeof(int));
            vector<string> requestsSent;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, []{ return !messageVectQueue.empty(); });  // Attend un message
                requestsSent = messageVectQueue.front();
                messageVectQueue.pop();
            }
            Menu menuFriendRequest(Menus::FriendRequestMenu, menuOptions, COLS, LINES, 50, 18);
            int returnValue = 0;
            int selected = 0;
            while(returnValue != -1){
                menuFriendRequest.displayFriendRequestsMenu(selected, requestsReceived, requestsSent); // Affiche le menu des demandes
                returnValue = handleFriendRequests(requestsReceived, requestsSent, selected);
            }
            break;
        }
        case Menus::LeaderboardMenu:
        {
            int type = MessageType::T_TopLeaderBoardRequest;
            int size = sizeof(int);
            sendAll(sock_, &type, sizeof(int));
            sendAll(sock_, &size, sizeof(int));
            std::vector<std::pair<std::string, int>> scores;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, []{ return !messageQueue.empty(); });  // Attend un message
                scores = messageQueue.front();
                messageQueue.pop();
            }

            vector<string> menuOptions = { "Leaderboard: Top Endless Mode Players" };
            Menu menuGameMode(Menus::LeaderboardMenu, menuOptions, COLS, LINES, 50, 12);
            menuGameMode.displayMenu(0, scores); // Affiche le menu principal
            break;
        }
        case Menus::LoginMenu:
        {
            vector<string> menuOptions = { "----- TETRIS ROYAL -----", "Log in", "Register" };
            Menu menuGameMode(Menus::LoginMenu, menuOptions, COLS, LINES, 60, 12);
            menuGameMode.displayMenu(selectedOption); // Affiche le menu principal
            break;
        }
        case Menus::maxMembers:
        {
            vector<string> menuOptions = {"Select the limit of players: ", "3", "4", "5", "6", "7", "8", "9" };
            Menu menuGameMode(Menus::LoginMenu, menuOptions, COLS, LINES, 80, 50);
            menuGameMode.displayMenu(selectedOption); // Affiche le menu principal
            break;
        }
        case Menus::FriendAddMenu:
        {
            enterName();
            break;
        }
        default:
        { 
            printw("Invalid menu choice");
            break;
        }
    }
}
/*/**
 *@brief Affiche le menu principal et l'option sélectionnée.
 * 
 *Affichage réalisé en parcourant les options du menu,
 *et une petite flèche pointe vers l'option actuelle.
 *
 *@param selectedOption Indice de l'option actuellement sélectionnée.
 */
 
 void InterfaceTerminal::displayMenu(int selectedOption) const {
    
    vector<string> menuOptions = { "Welcome to Tetris Royale (Terminal Edition)", "Play", "Handle friends", "Leaderboard (Endless Mode)" };
    Menu menuMain(Menus::MainMenu, menuOptions, COLS, LINES, 40, 10);
    menuMain.displayMenu(selectedOption); // Affiche le menu principal
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
    vector<string> menuOptions = { "----- TETRIS ROYAL -----", "Log in", "Register" };
    Menu menuGameMode(Menus::LoginMenu, menuOptions, COLS, LINES, 60, 12);
    menuGameMode.displayMenu(selectedOption); // Affiche le menu principal
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

    vector<string> menuOptions = { "Friends list:"};
    for (auto& friend_ : friends) {
        menuOptions.push_back(friend_);
    }
    Menu menuGameMode(Menus::FriendsMenu, menuOptions, COLS, LINES, 50, 20);


    int type = MessageType::T_NumberFriendRequestsRequest;
    int size = sizeof(int);
    sendAll(sock_, &type, sizeof(int));
    sendAll(sock_, &size, sizeof(int));
    int numberFRR;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, []{ return !messageIntQueue.empty(); });  // Attend un message
        numberFRR = messageIntQueue.front();
        messageIntQueue.pop();
    }

    menuGameMode.displayMenu(selectedOption, {}, numberFRR); // Affiche le menu principal
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

    printw("\nUse UP and DOWN arrow keys to navigate\nPress ENTER to select a friend\nPress 'BACKSPACE' to quit\n");
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
    while (!flag) {
        int type_ = MessageType::T_FriendListRequest;
        int size_ = sizeof(int);
        sendAll(sock_, &type_, sizeof(int));
        sendAll(sock_, &size_, sizeof(int));
        vector<string> friendsList;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, []{ return !messageVectQueue.empty(); });  // Attend un message
            friendsList = messageVectQueue.front();
            messageVectQueue.pop();
        }
        displayMessageBox(selectedOption, friendsList);  // affiche le menu avec les options
        nodelay(stdscr, true);
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
            case KEY_BACKSPACE:
            case 127:
                erase();
                nodelay(stdscr, true);
                refresh();
                endwin();
            return 0;
        }
    }
    nodelay(stdscr, true);
    erase();
    refresh();
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
void InterfaceTerminal::displayMessageNoRead(const std::string& user, const string& friendName, std::vector<std::string>& messagesNoRead){

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
    erase();
    refresh();
    int typeMes = MessageType::T_GetMessageNoReadRequest;
    int sizeMes = sizeof(int);
    sendAll(sock_, &typeMes, sizeof(int));
    sendAll(sock_, &sizeMes, sizeof(int));
    int lenMes = (int)friendName.size();
    sendAll(sock_, &lenMes, sizeof(lenMes));
    sendAll(sock_, friendName.c_str(), lenMes);
    vector<std::string> messagesNoRead;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, []{ return !messageVectQueue.empty(); });  // Attend un message
        messagesNoRead = messageVectQueue.front();
        messageVectQueue.pop();
    }
    refresh();
    interfaceMutex.lock();
    displayMessageNoRead(user, friendName, messagesNoRead);
    interfaceMutex.unlock();
    int type = MessageType::T_OpenConvRequest;
    int size = sizeof(int);
    sendAll(sock_, &type, sizeof(int));
    sendAll(sock_, &size, sizeof(int));
    int len_ = (int)friendName.size();
    sendAll(sock_, &len_, sizeof(len_));
    sendAll(sock_, friendName.c_str(), len_);
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
        if(strlen(buffer)==0){
            int type = MessageType::T_CloseConvRequest;
            int size = sizeof(int);
            sendAll(sock_, &type, sizeof(int));
            sendAll(sock_, &size, sizeof(int));
            int len_ = (int)friendName.size();
            sendAll(sock_, &len_, sizeof(len_));
            sendAll(sock_, friendName.c_str(), len_);
          return 0;
        }
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
        int type = MessageType::T_IsLoggedRequest;
        int size = sizeof(int);
        sendAll(sock_, &type, sizeof(int));
        sendAll(sock_, &size, sizeof(int));
        int len_ = (int)friendName.size();
        sendAll(sock_, &len_, sizeof(len_));
        sendAll(sock_, friendName.c_str(), len_);
        bool isLogged;
            {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, []{ return !messageIntQueue.empty(); });  // Attend un message
            isLogged = messageIntQueue.front();
            messageIntQueue.pop();
            }
        if(isLogged){
            int type = MessageType::T_IsInConvRequest;
            int size = sizeof(int);
            sendAll(sock_, &type, sizeof(int));
            sendAll(sock_, &size, sizeof(int));
            int len_ = (int)friendName.size();
            sendAll(sock_, &len_, sizeof(len_));
            sendAll(sock_, friendName.c_str(), len_);
            int isInConv;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, []{ return !messageIntQueue.empty(); });  // Attend un message
                isInConv = messageIntQueue.front();
                messageIntQueue.pop();
            }
        if(isInConv == 1){ //si le destinataire est sur la conv
        int messageSent = MessageType::T_Message;
        int size_ = sizeof(struct Message);
        Message messageToSent;
        std::strncpy(messageToSent.expediteur, user.c_str(), sizeof(messageToSent.expediteur) - 1);
        messageToSent.expediteur[sizeof(messageToSent.expediteur) - 1] =  '\0';
        std::strncpy(messageToSent.message, buffer, sizeof(messageToSent.message) - 1);
        messageToSent.message[sizeof(messageToSent.message) - 1] =  '\0';
        std::strncpy(messageToSent.destinataire, friendName.c_str(), sizeof(messageToSent.destinataire) - 1);
        messageToSent.destinataire[sizeof(messageToSent.destinataire) - 1] =  '\0';
        if(!sendAll(socket, &messageSent, sizeof(int))){flag=true; break;}
        if(!sendAll(socket, &size_, sizeof(int))){flag=true; break;}
        if(!sendAll(socket, &messageToSent, sizeof(Message))){flag=true; break;}
    }
    else{ //si le correspondant n'est pas dans la conv T_AddMessage
        std::string message(buffer, strnlen(buffer, sizeof(buffer)));
        int type = MessageType::T_AddMessage;
        int size = sizeof(int);
        sendAll(sock_, &type, sizeof(int));
        sendAll(sock_, &size, sizeof(int));
        int len_ = (int)friendName.size();
        sendAll(sock_, &len_, sizeof(len_));
        sendAll(sock_, friendName.c_str(), len_);
        int messageLen_ = (int)message.size();
        sendAll(sock_, &messageLen_, sizeof(messageLen_));
        sendAll(sock_, message.c_str(), messageLen_);
    }
}
        }
    }
    erase();
    refresh();
    int type_ = MessageType::T_CloseConvRequest;
    int size_ = sizeof(int);
    sendAll(sock_, &type_, sizeof(int));
    sendAll(sock_, &size_, sizeof(int));
    int len = (int)friendName.size();
    sendAll(sock_, &len, sizeof(len));
    sendAll(sock_, friendName.c_str(), len);
    return -1;
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
int InterfaceTerminal::handleFriendRequests(vector<string> requestsReceivedList, vector<string> requestsSentList, int& selectedOption) {
        int numberRequestReceived = std::size(requestsReceivedList);
        int numberRequestSent = std::size(requestsSentList);

        int ch = getch();    // récupère l'entrée utilisateur
        switch (ch) {

            case KEY_UP:     // aller vers le haut
                if (selectedOption > 0) {
                    selectedOption--;
                }
            break;
            case KEY_DOWN: // descendre
                if (selectedOption < numberRequestSent + numberRequestReceived-1) {
                    selectedOption++;
                }
            break;

            // touche 'entrée'
            case 'A':
            case 'a':
              if(selectedOption < numberRequestReceived){
                  int type = MessageType::T_AcceptRequestFriend;
                  int size = sizeof(int);
                  sendAll(sock_, &type, sizeof(int));
                  sendAll(sock_, &size, sizeof(int));
                  int len = (int)requestsReceivedList[selectedOption].size();
                  sendAll(sock_, &len, sizeof(len));
                  sendAll(sock_, requestsReceivedList[selectedOption].c_str(), len);
                  erase();
                  return -1;
              }
            break;
            case 'R':
            case 'r':
              if(selectedOption < numberRequestReceived){
                  int type = MessageType::T_RefuseRequestFriend;
                  int size = sizeof(int);
                  sendAll(sock_, &type, sizeof(int));
                  sendAll(sock_, &size, sizeof(int));
                  int len = (int)requestsReceivedList[selectedOption].size();
                  sendAll(sock_, &len, sizeof(len));
                  sendAll(sock_, requestsReceivedList[selectedOption].c_str(), len);
                  erase();
                  return -1;
              }
            break;
            case 'C':
            case 'c':
              if(numberRequestReceived <= selectedOption && selectedOption < numberRequestSent){
                  int type = MessageType::T_RefuseRequestFriend;
                  int size = sizeof(int);
                  sendAll(sock_, &type, sizeof(int));
                  sendAll(sock_, &size, sizeof(int));
                  int len = (int)requestsSentList[selectedOption].size();
                  sendAll(sock_, &len, sizeof(len));
                  sendAll(sock_, requestsSentList[selectedOption].c_str(), len);
                  erase();
                  return -1;
              }
            break;
            case KEY_BACKSPACE:
            case 127:
                erase();
                endwin();
            return -1;
        }
    return 0;
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
            curs_set(0);
            nodelay(stdscr, true);
            noecho();
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

        std::string user(namePlayer, strnlen(namePlayer, sizeof(namePlayer)));

        // Vérifier si la demande d'ami est possible
        int type = MessageType::T_VerifyRequestFriend;
        int size = sizeof(int);
        sendAll(sock_, &type, sizeof(int));
        sendAll(sock_, &size, sizeof(int));
        int len = (int)user.size();
        sendAll(sock_, &len, sizeof(len));
        sendAll(sock_, user.c_str(), len);


        int returnValue;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, []{ return !messageIntQueue.empty(); });  // Attend un message
            returnValue = messageIntQueue.front();
            messageIntQueue.pop();
        }
        erase();  // Efface tout avant d'afficher le résultat

        // Afficher le résultat de la recherche
        if (returnValue == 0) {
            int type = MessageType::T_SendRequestFriend;
            int size = sizeof(int);
            sendAll(sock_, &type, sizeof(int));
            sendAll(sock_, &size, sizeof(int));
            int len = (int)user.size();
            sendAll(sock_, &len, sizeof(len));
            sendAll(sock_, user.c_str(), len);
            int returnValue_;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, []{ return !messageIntQueue.empty(); });  // Attend un message
                returnValue_ = messageIntQueue.front();
                messageIntQueue.pop();
            }

            if (returnValue_) {
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
            int type = MessageType::T_AcceptRequestFriend;
            int size = sizeof(int);
            sendAll(sock_, &type, sizeof(int));
            sendAll(sock_, &size, sizeof(int));
            int len = (int)user.size();
            sendAll(sock_, &len, sizeof(len));
            sendAll(sock_, user.c_str(), len);
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
            erase();
            refresh();
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
                delwin(passwordWindow);
                break;  // Retour à la saisie du nom d'utilisateur
            }

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
//
//
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
            erase();
            refresh();
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


/***
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
            case KEY_BACKSPACE:
            case 127:
              int sock = getSock();
              int exitType = MessageType::T_Exit;
              int size = sizeof(int);
              sendAll(sock, &exitType, sizeof(int));
              sendAll(sock, &size, sizeof(int));
                endwin();
            return dataConnexion;
        }
    }
    return dataConnexion;
}
