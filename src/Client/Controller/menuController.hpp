#ifndef MENU_CONTROLLER_HPP
#define MENU_CONTROLLER_HPP

#include <string>
#include <vector>
#include <functional>
#include "menu.hpp" // Menu class (rendering part)
#include "InterFaceTerminal.hpp"
#include <memory>

using namespace std;





class MenuController {
    private:
        struct MenuOption {
            Menus menuName;
            std::vector<std::shared_ptr<MenuOption>> options;
            std::function<void()> callback;
        
            MenuOption() = default;
            MenuOption(Menus name, std::vector<std::shared_ptr<MenuOption>> opts, std::function<void()> cb)
                : menuName(name), options(opts), callback(cb) {}
        
            // Copy constructor
            // Constructeur de copie profond
            MenuOption(const MenuOption& other)
            : menuName(other.menuName), callback(other.callback) {
                options.reserve(other.options.size());
                for (const auto& opt : other.options) {
                    options.push_back(std::make_shared<MenuOption>(*opt)); // deep copy
                }
            }

            // Opérateur d’affectation profond
            MenuOption& operator=(const MenuOption& other) {
                if (this != &other) {
                    menuName = other.menuName;
                    callback = other.callback;
                    options.clear();
                    options.reserve(other.options.size());
                    for (const auto& opt : other.options) {
                        options.push_back(std::make_shared<MenuOption>(*opt)); // deep copy
                    }
                }
                return *this;
            }

        };

        InterfaceTerminal interfaceTerminal;
        bool runningMenu;
        MenuOption allMenus; 
        shared_ptr<MenuOption> currentMenu; // Current menu being displayed
        vector<int> selectedOptions; // Vector to store the selected options for each menu

        
        
    public:
        
        // Constructor
        MenuController(InterfaceTerminal interfaceTerminal);

        // Destructor
        ~MenuController() noexcept __attribute_noinline__ =  default;

        void handleMenu();
        void updateCurrentMenu();

        void handleKeyQ();
        void handleKeyEnter();
        
        void displayMenu();


};

#endif // MENU_CONTROLLER_HPP