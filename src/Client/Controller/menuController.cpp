#include "menuController.hpp"

MenuController::MenuController(InterfaceTerminal interfaceTerminal) : interfaceTerminal(interfaceTerminal), runningMenu(true) {
    interfaceTerminal.initNcurses();
    selectedOptions.push_back(0); // Initialize the selected options vector with the first menu option


    // Initialize the menu structure
    //allMenus = {
    //    Menus::LoginMenu,
    //    {
    //        {
    //            Menus::MainMenu, 
    //            {
    //                {
    //                    Menus::GameModeMenu, 
    //                    {
    //                        {Menus::LaunchEndless, {}, nullptr},
    //                        {Menus::JoinCreateMenu, {}, nullptr}
    //                    }, 
    //                    nullptr
    //                },
    //                {Menus::FriendsMenu, {}, nullptr},
    //                {Menus::LeaderboardMenu, {}, nullptr}
    //            }, nullptr
    //        }
    //    },
    //    nullptr
    //};
    allMenus = {
                Menus::MainMenu, 
                {
                    {
                        Menus::GameModeMenu, 
                        {
                            {Menus::LaunchEndless, {}, nullptr},
                            {Menus::JoinCreateMenu, {}, nullptr},
                            {Menus::JoinCreateMenu, {}, nullptr},
                            {Menus::JoinCreateMenu, {}, nullptr}
                        }, 
                        nullptr
                    },
                    {Menus::FriendsMenu, {}, nullptr},
                    {Menus::LeaderboardMenu, {}, nullptr}
                }, nullptr
    };
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
            case '\n':
                if (currentMenu.callback != nullptr) {
                    currentMenu.callback(); // Execute the callback function
                }
                else {
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

void MenuController::displayMenu() {
    updateCurrentMenu(); // Update the current menu based on the selected options
    interfaceTerminal.display(selectedOptions.back(), currentMenu.menuName);
}

void MenuController::updateCurrentMenu() {
    currentMenu = allMenus;
    for (int sub; sub < selectedOptions.size() - 1; sub++) {
        currentMenu = currentMenu.options[sub];
    }
    return;
}