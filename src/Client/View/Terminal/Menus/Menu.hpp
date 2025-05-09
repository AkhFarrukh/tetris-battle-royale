#ifndef MENU_HPP
#define MENU_HPP

#include <string.h>
#include <iostream>
#include <vector>
#include <ncurses.h>
#include <locale.h>  // Needed for wide characters (if printing Unicode blocks)

using namespace std;

enum class Menus {
    MainMenu,
    GameModeMenu,
    JoinCreateMenu,
    LoginMenu,
    LeaderboardMenu,
    FriendRequestMenu,
    FriendsMenu,
    
    maxMembers,

    LaunchEndless,

    JoinClassic,
    C_C3, C_C4, C_C5, C_C6, C_C7, C_C8, C_C9,
    CreateClassic,

    JoinDuel,
    C_D3, C_D4, C_D5, C_D6, C_D7, C_D8, C_D9,
    CreateDuel,

    JoinRoyalCompetition,
    C_R3, C_R4, C_R5, C_R6, C_R7, C_R8, C_R9,
    CreateRoyalCompetition,
};


class Menu
{
    private:
        Menus menuName;
        vector<string> menuOptions;
        int windowWidth;
        int windowHeight;
        int menuWidth;
        int menuHeight;
        int numberOfOptions;

    public:
        Menu(Menus menuName_, vector<string> menuOptions_, int windowWidth_, int windowHeight_, int menuWidth_, int menuHeight_);
        ~Menu() = default;

        void displayMenu(int optionHovered, std::vector<std::pair<std::string, int>> scores = {}, int numberFRR = 0);
        void displayMenuControls(WINDOW* menuWindow);
        void displayClassicMenu(WINDOW* menuWindow, int& optionHovered);
        void displayLeaderboard(WINDOW* menuWindow, std::vector<std::pair<std::string, int>>& scores);
        void displayFriendRequestsMenu(int selectedOption, vector<string>& requestsReceived, vector<string>& requestsSent);
        void displayFriendsMenu(WINDOW* menuWindow, int& optionHovered, int& numberFRR);

};






#endif