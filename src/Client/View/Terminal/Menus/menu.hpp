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

    LaunchEndless,
    JoinClassic,
    CreateClassic,
    JoinDuel,
    CreateDuel,
    JoinRoyalCompetition,
    CreateRoyalCompetition
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