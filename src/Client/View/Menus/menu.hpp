#ifndef MENU_HPP
#define MENU_HPP

#include <string.h>
#include <iostream>
#include <vector>
#include <ncurses.h>
#include <locale.h>  // Needed for wide characters (if printing Unicode blocks)

using namespace std;

class Menu
{
    private:
        string menuName;
        vector<string> menuOptions;
        int windowWidth;
        int windowHeight;
        int menuWidth;
        int menuHeight;
        int numberOfOptions;

    public:
        Menu(string menuName_, vector<string> menuOptions_, int windowWidth_, int windowHeight_, int menuWidth_, int menuHeight_);
        ~Menu() = default;

        void displayMenu(int menuHovered);

};



enum class Menus {

};


#endif