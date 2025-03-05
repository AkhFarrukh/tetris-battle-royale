//
// Created by farrukh on 25/02/25.
//

#include <memory>
#include "game.hpp"
#include "EventQueue.hpp"

#ifndef MENUGAMEMODECONTROLLER_HPP
#define MENUGAMEMODECONTROLLER_HPP



class MenuGameModeController {
    shared_ptr<EventQueue> eventQueue;
public:
    MenuGameModeController(shared_ptr<EventQueue> eventQueue);

    void startEndlessMode();

};



#endif //MENUGAMEMODECONTROLLER_HPP
