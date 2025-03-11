//
// Created by farrukh on 27/02/25.
//

#ifndef RENDERENGINENCURSES_HPP
#define RENDERENGINENCURSES_HPP

#include <ncurses.h>
#include <memory>
#include "GameController.hpp"
#include "game.hpp"

using namespace std;

class Game;

class RenderEngineNcurses {
private:
    static int applyColor(Color blockColor);

public:
    void renderGame(::std::shared_ptr<NetPlayerData> player, Coordinates offset);
    void renderGame(::std::shared_ptr<NetPlayerData> player);
    void renderGames(const vector<::std::shared_ptr<NetPlayerData>> &players);
    static void printGameOver();

};



#endif //RENDERENGINENCURSES_HPP
