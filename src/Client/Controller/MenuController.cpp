#include "MenuController.hpp"

MenuController::MenuController(InterfaceTerminal interfaceTerminal, std::shared_ptr<Player::ThreadData> threadData) : flagChangedMenu(true), interfaceTerminal(interfaceTerminal), runningMenu(true),
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
                        make_shared<MenuOption>(Menus::maxMembers, vector<shared_ptr<MenuOption>>{
                            make_shared<MenuOption>(Menus::C_C3, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_C4, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_C5, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_C6, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_C7, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_C8, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_C9, vector<shared_ptr<MenuOption>>{}, nullptr)
                        }, nullptr),
                        make_shared<MenuOption>(Menus::JoinClassic, vector<shared_ptr<MenuOption>>{}, nullptr)
                    }, nullptr),
                    make_shared<MenuOption>(Menus::JoinCreateMenu, vector<shared_ptr<MenuOption>>{
                        make_shared<MenuOption>(Menus::maxMembers, vector<shared_ptr<MenuOption>>{
                            make_shared<MenuOption>(Menus::C_R3, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_R4, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_R5, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_R6, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_R7, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_R8, vector<shared_ptr<MenuOption>>{}, nullptr),
                            make_shared<MenuOption>(Menus::C_R9, vector<shared_ptr<MenuOption>>{}, nullptr)
                        }, nullptr),
                        make_shared<MenuOption>(Menus::JoinRoyalCompetition, vector<shared_ptr<MenuOption>>{}, nullptr)
                    }, nullptr)
                },
                nullptr
            ),
            make_shared<MenuOption>(Menus::FriendsMenu, vector<shared_ptr<MenuOption>>{
                make_shared<MenuOption>(Menus::FriendAddMenu, vector<shared_ptr<MenuOption>>{}, nullptr),
                make_shared<MenuOption>(Menus::FriendRequestMenu, vector<shared_ptr<MenuOption>>{}, nullptr)
            }, nullptr),
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
        if (currentMenu != nullptr && (currentMenu->menuName == Menus::FriendRequestMenu || currentMenu->menuName == Menus::FriendAddMenu) && selectedOptions.size() > 1) {
            selectedOptions.pop_back(); // Go back to the previous menu
        }
        //nodelay(stdscr, false);
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                if (selectedOptions.back() > 0) {
                    selectedOptions.back()--; // Move up in the menu
                }
                break;
            case KEY_DOWN:
                if (selectedOptions.back() < static_cast<int>(currentMenu->options.size() - 1) || (currentMenu->menuName == Menus::FriendsMenu && selectedOptions.back() < static_cast<int>(currentMenu->numberOptions - 1))) {
                    selectedOptions.back()++; // Move down in the menu
                }
                break;
            case '\n':
                if(currentMenu->menuName == Menus::FriendsMenu){
                    if(selectedOptions.back() == currentMenu->numberOptions - 1){
                        selectedOptions.back()=1;
                        selectedOptions.push_back(0);
                        updateCurrentMenu(); // Update the current menu based on the selected options

                        flagChangedMenu = true;
                    }
                    else if(selectedOptions.back() == currentMenu->numberOptions - 2){
                        selectedOptions.back()=0;
                        selectedOptions.push_back(0);
                        updateCurrentMenu(); // Update the current menu based on the selected options

                        flagChangedMenu = true;
                    }
                }
                else if (currentMenu->callback != nullptr) {
                    currentMenu->callback(); // Execute the callback function
                }
                else if (!currentMenu->options.empty()) {
                    selectedOptions.push_back(0); 
                    updateCurrentMenu(); // Update the current menu based on the selected options

                    flagChangedMenu = true; // The menu has changed
                }
                break;
            case KEY_BACKSPACE:
            case 127: // backspace key for macOS
                if (selectedOptions.size() > 1) {
                    selectedOptions.pop_back(); // Go back to the previous menu
                    flagChangedMenu = true; // The menu has changed
                } 
                else {
                    runningMenu = false; // Exit the menu
                }
                break;
            case 'T':
            case 't':
                interfaceTerminal.handleMessageBox();
            break;
            default:
                break;
        }

        Menus leaf = currentMenu->menuName;

        switch (leaf) {
            case Menus::LaunchEndless:
                comm.sendMenuRequest(MenuActions::A_Play, Modes::Endless_, 1);
                threadData->userInGame  = true;
                break;

            // Classic submenu leaves
            case Menus::C_C3: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Classic_, 3); threadData->userInLobby = true; break;
            case Menus::C_C4: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Classic_, 4); threadData->userInLobby = true; break;
            case Menus::C_C5: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Classic_, 5); threadData->userInLobby = true; break;
            case Menus::C_C6: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Classic_, 6); threadData->userInLobby = true; break;
            case Menus::C_C7: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Classic_, 7); threadData->userInLobby = true; break;
            case Menus::C_C8: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Classic_, 8); threadData->userInLobby = true; break;
            case Menus::C_C9: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Classic_, 9); threadData->userInLobby = true; break;
            case Menus::JoinClassic:
                comm.sendMenuRequest(MenuActions::A_JoinRoom,   Modes::Classic_, -1);
                threadData->userInLobby = true;
                break;

            // Duel submenu leaves
            case Menus::CreateDuel:
                comm.sendMenuRequest(MenuActions::A_CreateRoom,   Modes::Duel_, 2);
                threadData->userInLobby = true;
                break;
            case Menus::JoinDuel:
                comm.sendMenuRequest(MenuActions::A_JoinRoom,   Modes::Duel_, -1);
                threadData->userInLobby = true;
                break;

            // Royal Competition submenu leaves
            case Menus::C_R3: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Royal_, 3); threadData->userInLobby = true; break;
            case Menus::C_R4: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Royal_, 4); threadData->userInLobby = true; break;
            case Menus::C_R5: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Royal_, 5); threadData->userInLobby = true; break;
            case Menus::C_R6: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Royal_, 6); threadData->userInLobby = true; break;
            case Menus::C_R7: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Royal_, 7); threadData->userInLobby = true; break;
            case Menus::C_R8: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Royal_, 8); threadData->userInLobby = true; break;
            case Menus::C_R9: comm.sendMenuRequest(MenuActions::A_CreateRoom, Modes::Royal_, 9); threadData->userInLobby = true; break;
            case Menus::JoinRoyalCompetition:
                comm.sendMenuRequest(MenuActions::A_JoinRoom,   Modes::Royal_, -1);
                threadData->userInLobby = true;
                break;

            default:
                break;
        }
        
        if (threadData->userInGame || threadData->userInLobby) {
            Player::sendInput(threadData);
            selectedOptions.pop_back();
        }
        
    }
}

void MenuController::displayMenu() {
    updateCurrentMenu(); // Update the current menu based on the selected options
    int number = 0;
    interfaceTerminal.display(selectedOptions.back(), currentMenu->menuName, flagChangedMenu, number);
    if(currentMenu->menuName == Menus::FriendsMenu)currentMenu->numberOptions = number;

}

void MenuController::updateCurrentMenu() {
    currentMenu = make_shared<MenuOption>(allMenus);
    for (int sub = 0; sub < static_cast<int>(selectedOptions.size() - 1); sub++) {
        currentMenu = currentMenu->options[selectedOptions[sub]];
    }
    return;
}