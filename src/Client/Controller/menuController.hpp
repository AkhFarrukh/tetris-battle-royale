#ifndef MENU_CONTROLLER_HPP
#define MENU_CONTROLLER_HPP

#include <string>
#include <vector>
#include <functional>
#include "menu.hpp" // Menu class (rendering part)
#include "InterFaceTerminal.hpp"

using namespace std;

struct MenuOption {
    Menus menuName;
    std::vector<MenuOption> options; // Vector to store sub-menus options
    std::function<void()> callback; // Callback function to be executed when the option is selected
};

class MenuController {
    private:
        
        InterfaceTerminal interfaceTerminal;
        bool runningMenu;
        MenuOption allMenus; 
        MenuOption currentMenu; // Current menu being displayed
        vector<int> selectedOptions; // Vector to store the selected options for each menu

        
        
    public:
        
        // Constructor
        MenuController(InterfaceTerminal interfaceTerminal);

        // Destructor
        ~MenuController() = default;

        void handleMenu();
        void updateCurrentMenu();

        void handleKeyQ();
        void handleKeyEnter();
        
        void displayMenu();


};

#endif // MENU_CONTROLLER_HPP