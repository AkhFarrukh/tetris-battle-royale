//
// Created by farrukh on 26/02/25.
//

#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include "input.hpp"
#include "ControllerEnum.hpp"
#include "game.hpp"
#include "RenderEngineNcurses.hpp"
#include "EventQueue.hpp"
#include "MovementActionEvent.hpp"
using namespace std;


class Game; // foreward declaration

//todo remake with observer pattern maybe?

class GameController {
private:
    shared_ptr<Game> game;
    shared_ptr<EventQueue> eventQueue;
public:
    GameController(shared_ptr<EventQueue> eventQueue);
    void getAction(KeyboardAction in);

    /*
    int getGameScore();
    int getGameNumberOfBrokenLines();
    Color getBlockColor(Coordinates cell);
    */
    void renderGame();
    void printGameOver();

};


#endif //GAMECONTROLLER_HPP
