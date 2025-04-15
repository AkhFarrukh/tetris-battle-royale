#include "menu.hpp"


Menu::Menu(Menus menuName_, vector<string> menuOptions_, int windowWidth_, int windowHeight_, int menuWidth_, int menuHeight_) :
    menuName(menuName_), menuOptions(menuOptions_), windowWidth(windowWidth_), windowHeight(windowHeight_), menuWidth(menuWidth_), menuHeight(menuHeight_)
    {
        numberOfOptions = menuOptions.size();
    }

/**
 * @brief Affiche le menu 
 * 
 * @param optionHovered option qui est "surlignée"
 */
void Menu::displayMenu(int optionHovered)
{
    // position de depart du menu pour un bon cadrage
    int startX = (windowWidth - menuWidth) / 2;
    int startY = (windowHeight - menuHeight) / 2;

    string title = menuOptions[0]; // titre du menu
    int titleLength = title.size(); // longueur du titre

    attron(A_BOLD); // mettre en gras (via ncurses)
    mvprintw(startY - 8, (windowWidth - titleLength) / 2, "%s", title.c_str());  // titre Afficher au-dessus du cadre
    attroff(A_BOLD);

    // creation d'une fenetre pour le menu ( avec un cadre/bordure)
    WINDOW* menuWindow = newwin(menuHeight, menuWidth, startY, startX);
    box(menuWindow, 0, 0);  // dessine le cadre

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
    displayMenuControls(menuWindow); // Afficher les controles du menu
    wrefresh(menuWindow); // Refresh la window
    refresh();
    delwin(menuWindow); // detruire pour liberer memoire
}


void Menu::displayMenuControls(WINDOW* menuWindow)
{
    if (menuName == Menus::LoginMenu)
    {  
        mvwprintw(menuWindow, 1, 2, "Log in or Register here:");
    }
    else
    {

    }
    int startY = (windowHeight - menuHeight) / 2;
    mvprintw(startY + menuHeight+ 3, (windowWidth - 45) / 2, "Use UP and DOWN arrow keys to navigate");           // TODO -- gerer les nombres magiques présents (qui permettent le bon cadrage des elements)
    mvprintw(startY + menuHeight + 4, (windowWidth - 45) / 2, "Press ENTER to select an action");
    mvprintw(startY + menuHeight + 5, (windowWidth - 45) / 2, "Press 'q' to quit");
    mvprintw(startY + menuHeight + 6, (windowWidth - 45) / 2, "Press 't' to open the chat");
}