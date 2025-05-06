#ifndef MENU_CONTROLLER_HPP
#define MENU_CONTROLLER_HPP

#include <string>
#include <vector>
#include <functional>
#include "menu.hpp" // Menu class (rendering part)




class MenuController {
    private:
        struct MenuOption {
            Menus menuName;
            std::vector<MenuOption> options; // Vector to store sub-menus options
            std::function<void()> callback; // Callback function to be executed when the option is selected
        };
        
    public:
        // Constructor
        MenuController();

        // Destructor
        ~MenuController();

        // Adds a menu option with a label and a callback function
        void addOption(const std::string& label, const std::function<void()>& callback);

        // Displays the menu and handles user input
        void displayMenu() const;


};

#endif // MENU_CONTROLLER_HPP