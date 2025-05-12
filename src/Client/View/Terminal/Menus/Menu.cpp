#include "Menu.hpp"


Menu::Menu(Menus menuName_, vector<string> menuOptions_, int windowWidth_, int windowHeight_, int menuWidth_, int menuHeight_) :
    menuName(menuName_), menuOptions(menuOptions_), windowWidth(windowWidth_), windowHeight(windowHeight_), menuWidth(menuWidth_), menuHeight(menuHeight_)
    {
        numberOfOptions = static_cast<int>(menuOptions.size());
    }

/**
 * @brief Affiche le menu 
 * 
 * @param optionHovered option qui est "surlignée"
 */
void Menu::displayMenu(int optHov, std::vector<std::pair<std::string, int>> scores, int numberFRR)
{

    // Correction optionHovered
    int optionHovered = optHov + 1;

    // position de depart du menu pour un bon cadrage
    int startX = (windowWidth - menuWidth) / 2;
    int startY = (windowHeight - menuHeight) / 2;

    string title = menuOptions[0]; // titre du menu
    int titleLength = static_cast<int>(title.size()); // longueur du titre

    attron(A_BOLD); // mettre en gras (via ncurses)
    mvprintw(startY - 8, (windowWidth - titleLength) / 2, "%s", title.c_str());  // titre Afficher au-dessus du cadre
    attroff(A_BOLD);

    // creation d'une fenetre pour le menu ( avec un cadre/bordure)
    WINDOW* menuWindow = newwin(menuHeight, menuWidth, startY, startX);
    box(menuWindow, 0, 0);  // dessine le cadre

    if (menuName == Menus::LeaderboardMenu)
    {
        displayLeaderboard(menuWindow, scores); // Afficher le leaderboard
    }
    else if (menuName == Menus::FriendsMenu)
    {
        displayClassicMenu(menuWindow, optionHovered);
        displayFriendsMenu(menuWindow, optionHovered, numberFRR); // Afficher le menu des amis
    }
    else 
    {
        displayClassicMenu(menuWindow, optionHovered); // Afficher le menu classique
    }
    displayMenuControls(menuWindow); // Afficher les controles du menu
    wrefresh(menuWindow); // Refresh la window
    refresh();
    delwin(menuWindow); // detruire pour liberer memoire
}

/**
 * @brief affiche les controles du menu
 * 
 * @param menuWindow 
 */
void Menu::displayMenuControls(WINDOW* menuWindow)
{
    int startY = (windowHeight - menuHeight) / 2;
    if (menuName == Menus::LoginMenu)
    {  
        mvwprintw(menuWindow, 1, 2, "Log in or Register here:");
        mvprintw(startY + menuHeight+ 3, (windowWidth - 45) / 2, "Use UP and DOWN arrow keys to navigate");           // TODO -- gerer les nombres magiques présents (qui permettent le bon cadrage des elements)
        mvprintw(startY + menuHeight + 4, (windowWidth - 45) / 2, "Press ENTER to select an action");
        mvprintw(startY + menuHeight + 5, (windowWidth - 45) / 2, "Press 'BACKSPACE' to quit");
    }
    else if(menuName == Menus::FriendRequestMenu)
    {
        mvprintw(startY + menuHeight + 1, (COLS - 50) / 2, "Use UP and DOWN arrow keys to navigate");
        mvprintw(startY + menuHeight + 2, (COLS - 50) / 2, "Press 'A' to accept the request");
        mvprintw(startY + menuHeight + 3, (COLS - 50) / 2, "Press 'R' to refuse");
        mvprintw(startY + menuHeight + 4, (COLS - 50) / 2, "Press 'C' to cancel a sent friend request");
        mvprintw(startY + menuHeight + 5, (COLS - 50) / 2, "Press 'BACKSPACE' to quit");
    }
    else 
    {
        mvprintw(startY + menuHeight+ 3, (windowWidth - 45) / 2, "Use UP and DOWN arrow keys to navigate");           // TODO -- gerer les nombres magiques présents (qui permettent le bon cadrage des elements)
        mvprintw(startY + menuHeight + 4, (windowWidth - 45) / 2, "Press ENTER to select an action");
        mvprintw(startY + menuHeight + 5, (windowWidth - 45) / 2, "Press 'BACKSPACE' to quit");
        mvprintw(startY + menuHeight + 6, (windowWidth - 45) / 2, "Press 't' to open the chat");
    }
}

/**
 * @brief Affiche le menu classique
 * 
 * @param menuWindow 
 * @param optionHovered option qui est "surlignée"
 */
void Menu::displayClassicMenu(WINDOW* menuWindow, int& optionHovered)
{
    // Display menu options
    for (int i = 1; i < numberOfOptions; ++i) {
        if (i == optionHovered) {
            // met en evidence l'option selectionnée
            wattron(menuWindow, COLOR_PAIR(9));   // pour activer  mise en blanc de l'option
            mvwprintw(menuWindow, 3 + (i - 1) * 2, 2, "-> %d. %s ", i, menuOptions[i].c_str());    // ' * 2 '  pour un avoir espacer un peu plus entre options
            wattroff(menuWindow, COLOR_PAIR(9)); 
        } else {
            mvwprintw(menuWindow, 3 + (i - 1)*2 , 2, "   %d. %s ", i, menuOptions[i].c_str());
        }
    }
}

/**
 * @brief Affiche le leaderboard
 * 
 * @param menuWindow 
 * @param scores contient les meilleurs scores
 */
void Menu::displayLeaderboard(WINDOW* menuWindow, std::vector<std::pair<std::string, int>>& scores)
{
    // Récupération des scores
    if (scores.empty()) {
        mvwprintw(menuWindow, 3, 2, "No scores recorded yet.");
    } else {
        for (size_t i = 0; i < scores.size(); i++) {
            mvwprintw(menuWindow, static_cast<int>(3 + i), 2, "%d. %-15s - %d points", static_cast<int>(i + 1), scores[i].first.c_str(), scores[i].second);
        }
    }
    wrefresh(menuWindow); // Refresh la window
    refresh();
}

/**
 * @brief Affiche le menu des demandes d'amis
 * 
 * @param selectedOption option qui est "surlignée"
 * @param requestsReceived demandes d'amis reçues
 * @param requestsSent demandes d'amis envoyées
 */
void Menu::displayFriendRequestsMenu(int selectedOption, vector<string>& requestsReceived, vector<string>& requestsSent)
{
    int startX = (COLS - menuWidth)/ 2;
    int startY = (LINES - menuHeight - 10) / 2;

    // Créer une fenêtre avec un grand cadre
    WINDOW* requestWindow = newwin(menuHeight, menuWidth, startY, startX);
    box(requestWindow, 0, 0);

    vector<vector<string>> requests = { requestsSent, requestsReceived };
    vector<string> requestsReceivedSentences = {"No request received !", "-> Friend request sent by ", "   Friend request sent by " };
    vector<string> requestsSentSentences = {"No requests sent !", "-> Friend request sent to ", "   Friend request sent to " };
    vector<vector<string>> requestsSentences = { requestsSentSentences, requestsReceivedSentences };
    for (int j = 0; j<static_cast<int>(menuOptions.size()-1); j++)
    {
        // Titre "Requests received" en vert
        wattron(requestWindow, COLOR_PAIR(2) | A_BOLD);
        mvwprintw(requestWindow, (9 * j) + 1, 2, "%s", menuOptions[j].c_str());
        wattroff(requestWindow, COLOR_PAIR(2) | A_BOLD);

        int numberRequests = requests[j].size();
        if (numberRequests == 0) {
            mvwprintw(requestWindow, (9 * j) + 3, 4, "%s", requestsSentences[j][0].c_str());
        } else {
            for (int i = 0; i < numberRequests; ++i) {
                std::string user;
                if(j == 0)user=requestsSent[i];
                else user=requestsReceived[i];
                if ((i == selectedOption && j == 0) || (i == selectedOption - requestsSent.size() && j == 1)) {
                    wattron(requestWindow, A_REVERSE);
                    mvwprintw(requestWindow, (9 * j) + 3 + i, 4, "%s %s", requestsSentences[j][1].c_str(), user.c_str());
                    wattroff(requestWindow, A_REVERSE);
                } else {
                    mvwprintw(requestWindow, (9 * j) + 3 + i, 4, "%s %s", requestsSentences[j][2].c_str(), user.c_str());

                }
            }
        }
    }
    displayMenuControls(requestWindow);
    wrefresh(requestWindow);
    refresh();
    delwin(requestWindow);
}

/**
 * @brief S'occupe des particularité d'affichage du friends menu
 * 
 * @param menuWindow 
 * @param optionHovered 
 * @param numberFRR friend requests received number
 */
void Menu::displayFriendsMenu(WINDOW* menuWindow, int& optionHovered, int& numberFRR)
{
    int bottomY = menuHeight - 4;
    if (optionHovered == numberOfOptions) {
        wattron(menuWindow, COLOR_PAIR(9));
        mvwprintw(menuWindow, bottomY, 2, "-> Add new friend");
        wattroff(menuWindow, COLOR_PAIR(9));
    }
    else {
        mvwprintw(menuWindow, bottomY, 2, "   Add new friend");
    }

    if (optionHovered == numberOfOptions + 1) {
        wattron(menuWindow, COLOR_PAIR(9));
        mvwprintw(menuWindow, bottomY + 2, 2, "-> Friend requests (%d)", numberFRR);
        wattroff(menuWindow, COLOR_PAIR(9));
    }
    else {
        mvwprintw(menuWindow, bottomY + 2, 2, "   Friend requests (%d)", numberFRR);
    }
}





