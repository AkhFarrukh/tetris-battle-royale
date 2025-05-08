#include "menuController.hpp"

MenuController::MenuController() : runningMenu(true) {
}

void MenuController::handleMenu() {
    while (runningMenu) {
        //displayMenu();
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                // Handle up arrow key
                break;
            case KEY_DOWN:
                // Handle down arrow key
                break;
            case '\n':
                // Handle enter key
                break;
            case 'q':
            case 'Q':
                runningMenu = false; // Exit the menu loop
                break;
            default:
                break;
        }
    }
}