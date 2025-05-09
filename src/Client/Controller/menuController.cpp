#include "menuController.hpp"

MenuController::MenuController(InterfaceTerminal interfaceTerminal, std::shared_ptr<Player::ThreadData> threadData) : interfaceTerminal(interfaceTerminal), runningMenu(true),
 threadData(threadData) {
    interfaceTerminal.initNcurses();

    allMenus = MenuOption(
        Menus::MainMenu,
        {
            make_shared<MenuOption>(Menus::GameModeMenu,
                vector<shared_ptr<MenuOption>>{
                    make_shared<MenuOption>(Menus::LaunchEndless, vector<shared_ptr<MenuOption>>{}, nullptr),
                    make_shared<MenuOption>(Menus::JoinCreateMenu, vector<shared_ptr<MenuOption>>{
                        make_shared<MenuOption>(Menus::CreateDuel, vector<shared_ptr<MenuOption>>{}, nullptr),
                        make_shared<MenuOption>(Menus::JoinDuel, vector<shared_ptr<MenuOption>>{}, nullptr)
                    }, nullptr),
                    make_shared<MenuOption>(Menus::JoinCreateMenu, vector<shared_ptr<MenuOption>>{
                        make_shared<MenuOption>(Menus::CreateClassic, vector<shared_ptr<MenuOption>>{}, nullptr),
                        make_shared<MenuOption>(Menus::JoinClassic, vector<shared_ptr<MenuOption>>{}, nullptr)
                    }, nullptr),
                    make_shared<MenuOption>(Menus::JoinCreateMenu, vector<shared_ptr<MenuOption>>{
                        make_shared<MenuOption>(Menus::CreateRoyalCompetition, vector<shared_ptr<MenuOption>>{}, nullptr),
                        make_shared<MenuOption>(Menus::JoinRoyalCompetition, vector<shared_ptr<MenuOption>>{}, nullptr)
                    }, nullptr)
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
    comm.setSock(threadData->servSocket); 
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

        Menus leaf = currentMenu->menuName;

        switch (leaf) {
            case Menus::LaunchEndless:
                comm.sendMenuRequest(MenuActions::A_Play, Modes::Endless_);
                threadData->userInLobby = false;
                threadData->userInGame  = true;
                break;

            // Classic submenu leaves
            case Menus::CreateClassic:
                comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Classic_);
                threadData->userInLobby = true;
                break;
            case Menus::JoinClassic:
                comm.sendMenuRequest(MenuActions::A_JoinRoom,   Modes::Classic_);
                threadData->userInLobby = true;
                break;

            // Duel submenu leaves
            case Menus::CreateDuel:
                comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Duel_);
                threadData->userInLobby = true;
                break;
            case Menus::JoinDuel:
                comm.sendMenuRequest(MenuActions::A_JoinRoom,   Modes::Duel_);
                threadData->userInLobby = true;
                break;

            // Royal Competition submenu leaves
            case Menus::CreateRoyalCompetition:
                comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Royal_);
                threadData->userInLobby = true;
                break;
            case Menus::JoinRoyalCompetition:
                comm.sendMenuRequest(MenuActions::A_JoinRoom,   Modes::Royal_);
                threadData->userInLobby = true;
                break;

            default:
                break;
        }

        clear(); 
        refresh();

        while (threadData->userInLobby || threadData->userInGame) {
            napms(50);
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