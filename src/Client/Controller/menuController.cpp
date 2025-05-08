#include "menuController.hpp"

MenuController::MenuController() : runningMenu(true) {
}

void MenuController::handleMenu() {
    while (runningMenu) {
        displayMenu();
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                if (selectedOptions.back() > 0) {
                    selectedOptions.back()--; // Move up in the menu
                }
                break;
            case KEY_DOWN:
                if (selectedOptions.back() < allMenus.options.size() - 1) {
                    selectedOptions.back()++; // Move down in the menu
                }
                break;
            case KEY_ENTER:
                if (currentMenu.callback != nullptr) {
                    currentMenu.callback(); // Execute the callback function
                }
                else {
                    currentMenu = currentMenu.options[selectedOptions.back()]; // Go to the selected sub-menu
                    selectedOptions.push_back(0); 
                }
                break;
            case KEY_BACKSPACE:
                if (selectedOptions.size() > 1) {
                    selectedOptions.pop_back(); // Go back to the previous menu
                } 
                else {
                    runningMenu = false; // Exit the menu
                }
                break;
            default:
                break;
        }
    }
}

void MenuController::displayMenu() const {
    // Display the current menu
    // This function should render the menu using the Menu class
    // For example:
    // currentMenu.displayMenu(selectedOptions.back());
}