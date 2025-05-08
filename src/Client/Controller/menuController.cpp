#include "menuController.hpp"

MenuController::MenuController(InterfaceTerminal interfaceTerminal) : interfaceTerminal(interfaceTerminal), runningMenu(true) {
    interfaceTerminal.initNcurses();

    allMenus = MenuOption(
        Menus::MainMenu,
        {
            make_shared<MenuOption>(
                Menus::GameModeMenu,
                vector<shared_ptr<MenuOption>>{
                    make_shared<MenuOption>(Menus::LaunchEndless, vector<shared_ptr<MenuOption>>{}, nullptr),
                    make_shared<MenuOption>(Menus::JoinCreateMenu, vector<shared_ptr<MenuOption>>{}, nullptr),
                    make_shared<MenuOption>(Menus::LaunchRoyalCompetition, vector<shared_ptr<MenuOption>>{}, nullptr),
                    make_shared<MenuOption>(Menus::LaunchDuel, vector<shared_ptr<MenuOption>>{}, nullptr)
                },
                nullptr
            ),
            make_shared<MenuOption>(Menus::FriendsMenu, vector<shared_ptr<MenuOption>>{}, nullptr),
            make_shared<MenuOption>(Menus::LeaderboardMenu, vector<shared_ptr<MenuOption>>{}, nullptr)
        },
        nullptr
    );    
    currentMenu = make_shared<MenuOption>(allMenus); // Initialize the current menu to the main menu
    selectedOptions.push_back(0); // Initialize the selected options vector with the first menu option

    
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
                if (selectedOptions.back() < static_cast<int>(currentMenu->options.size() - 1)) {
                    selectedOptions.back()++; // Move down in the menu
                }
                break;
            case '\n':
                if (currentMenu->callback != nullptr) {
                    currentMenu->callback(); // Execute the callback function
                }
                else if (!currentMenu->options.empty()) {
                    selectedOptions.push_back(0); 
                    updateCurrentMenu(); // Update the current menu based on the selected options
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

void MenuController::displayMenu() {
    updateCurrentMenu(); // Update the current menu based on the selected options
    interfaceTerminal.display(selectedOptions.back(), currentMenu->menuName);
}

void MenuController::updateCurrentMenu() {
    currentMenu = make_shared<MenuOption>(allMenus);
    for (int sub = 0; sub < static_cast<int>(selectedOptions.size() - 1); sub++) {
        currentMenu = currentMenu->options[selectedOptions[sub]];
    }
    return;
}