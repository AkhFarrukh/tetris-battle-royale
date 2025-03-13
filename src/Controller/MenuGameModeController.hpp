//
// Created by farrukh on 25/02/25.
//

#ifndef MENUGAMEMODECONTROLLER_HPP
#define MENUGAMEMODECONTROLLER_HPP


#include <memory>
#include "game.hpp"
#include "EventQueue.hpp"


class MenuGameModeController {
    shared_ptr<EventQueue> eventQueue;
public:
    MenuGameModeController(shared_ptr<EventQueue> eventQueue);

    void startEndlessMode();

};



#endif //MENUGAMEMODECONTROLLER_HPP
