#include "InterfaceTerminal.hpp"
#include <ncurses.h>
#include <locale.h>  // Needed for wide characters (if printing Unicode blocks)


atomic<bool> flag = false; // informer thread de se terminer (Ctrl + C notamment)
std::mutex interfaceMutex;

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


void InterfaceTerminal::display(int selectedOption, Menus menuChoosen) {
    refresh();
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
            vector<string> friendsList = udm.getFriendsList(user);
            displayFriendsMenu(selectedOption, friendsList); // Affiche le menu des amis
            break;
        }
        case Menus::FriendRequestMenu:
        {
            vector<string> menuOptions = { "Friend Requests", "Received requests", "Sent requests" };
            vector<string> requestsReceived = udm.getReceivedRequestsList(user);
            vector<string> requestsSent = udm.getSentRequestsList(user);
            Menu menuFriendRequest(Menus::FriendRequestMenu, menuOptions, COLS, LINES, 50, 18);
            menuFriendRequest.displayFriendRequestsMenu(selectedOption, requestsReceived, requestsSent); // Affiche le menu des demandes
            break;
        }
        case Menus::LeaderboardMenu:
        {
            std::vector<std::pair<std::string, int>> scores = udm.getTopScores(5);
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
            Menu menuGameMode(Menus::LoginMenu, menuOptions, COLS, LINES, 60, 12);
            menuGameMode.displayMenu(selectedOption); // Affiche le menu principal
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

    int numberFRR = udm.getNumberFriendRequestsReceived(user);

    menuGameMode.displayMenu(selectedOption, {}, numberFRR); // Affiche le menu principal
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
            case 'q':
            case 'Q':
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
