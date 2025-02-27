//
// Created by farrukh on 27/02/25.
//

#ifndef RENDERENGINENCURSES_HPP
#define RENDERENGINENCURSES_HPP

#include <ncurses.h>
#include <memory>
#include "GameController.hpp"

using namespace std;

class Game;

class RenderEngineNcurses {
private:
    static int applyColor(Color blockColor);
public:
    static void renderGame(shared_ptr<Game> game);
    static void printGameOver();

};



#endif //RENDERENGINENCURSES_HPP
