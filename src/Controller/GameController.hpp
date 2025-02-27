//
// Created by farrukh on 26/02/25.
//

#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include "input.hpp"
#include "ControllerEnum.hpp"
#include "game.hpp"
using namespace std;


class Game; // foreward declaration



class GameController {
private:
    Input input;
    shared_ptr<Game> game;
public:
    GameController(shared_ptr<Game> game);
    void startGameInput() {input.start();}
    void stopGameInput() {input.stop();}
    Action getAction();

    int getGameScore();
    int getGameNumberOfBrokenLines();
    Color getBlockColor(Coordinates cell);

};


#endif //GAMECONTROLLER_HPP
